/**
@file camera.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief
@Date 9/10/2020

Note: 
Lines 154, 219, 248, 517, and 690-698 all contain commented out code 
**/
#include <camera.h>
#include <main.h>
#include <housekeeping_extern.h>
#include <macros.h>
#include <unistd.h>
#include <fitsio.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <omp.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <pthread.h>


double minShortExposure = -1 ;

double maxShortExposure = -1;

unsigned pix_bin  = PIX_BIN;


//routine declarations
void convert_to_packet(image * a , datavis_p * b)
{
	b -> tnow = a -> tnow ;
	b -> exposure = (a -> exposure) ;
	b -> pixx = 348 ;
	b -> pixy = 260 ;
	b -> pixbin = pix_bin ;
	b -> ccdtemp = a -> ccdtemp ;
	b -> boardtemp = a -> boardtemp ;
	b -> chassistemp = a -> chassistemp ;
	uint8_t numbin = (a->pixx) / (b->pixx) ;
	if (numbin == 1) //no downsample, just copy
		for (int i = 0 ; i < a -> imgsize ; i++ )
			b->picdata[i] = (unsigned char)((a->picdata[i]) / 256);
	else {
		for (unsigned short i = 0 ; i < 260 ; i++ )
		{
			for (unsigned short j = 0 ; j < 348 ; j++)
			{
				double temp = 0 ;
				for (unsigned char k = 0 ; k < numbin; k++)
					for ( unsigned char l = 0 ; l < numbin ; l++ )
						temp += a->picdata[i*numbin*348*numbin + j*numbin /*root of pixel*/
						+ 348*numbin*k /*y axis*/
						+l /* x axis */]*255.0/65535; //converted to 8 bit
				temp /= numbin * numbin ;
				b->picdata[i*348+j] = (unsigned char)temp;
			}
		}
	}
	return ;
}


inline void put_data ( ostream & str , unsigned short val )
{
	shb x ;
	x.s = val ;
	for ( char i = 0 ; i < sizeof(x.b) ; i++ )
		str << x.b[i] ;
}
inline void put_data ( ostream & str , unsigned long long int val )
{
	llb x ;
	x.l = val ;
	for ( char i = 0 ; i < sizeof(x.b) ; i++ )
		str << x.b[i] ;
}
inline void put_data ( ostream & str , float val )
{
	flb x ;
	x.f = val ;
	for ( char i = 0 ; i < sizeof(x.b) ; i++ )
		str << x.b[i] ;
}


int save(const char *fileName , image * data) {
  fitsfile *fptr;
  int status = 0, bitpix = USHORT_IMG, naxis = 2;
  int bzero = 32768, bscale = 1;
  long naxes[2] = { (long)data->pixx, (long)data->pixy };
  if (!fits_create_file(&fptr, fileName, &status)) {
	fits_set_compression_type(fptr, RICE_1, &status) ;
    fits_create_img(fptr, bitpix, naxis, naxes, &status);
    fits_write_key(fptr, TSTRING, "PROGRAM", (void *)"sk_flight", NULL, &status);
    fits_write_key(fptr, TUSHORT, "BZERO", &bzero, NULL, &status);
    fits_write_key(fptr, TUSHORT, "BSCALE", &bscale, NULL, &status);
    fits_write_key(fptr, TFLOAT, "EXPOSURE", &(data->exposure), NULL, &status);
    fits_write_key(fptr, TSHORT, "TEMP", &(data->ccdtemp), NULL, &status);
    fits_write_key(fptr, TLONGLONG, "TIMESTAMP", &(data->tnow),NULL, &status);
    long fpixel[] = { 1, 1 };
    fits_write_pix(fptr, TUSHORT, fpixel, data->imgsize, data->picdata, &status);
    fits_close_file(fptr, &status);
	#ifdef SK_DEBUG
    cerr << endl << "Camera Thread: Save: saved to " << fileName << endl << endl;
	#endif
  }
  
#ifdef ENABLE_REBOOT
void sys_reboot(void)
{
	cerr << "Info: Reboot instruction received!" << endl ;
	sync() ;
	setuid(0) ;
	reboot(RB_AUTOBOOT) ;
}
#else
void sys_reboot(void)
{
	cerr << "Info: Reboot instruction received!" << endl ;
}
#endif //ENABLE_REBOOT
  
char space_left(void)
{
	boost::filesystem::space_info si = boost::filesystem::space(curr_dir) ;
	#ifdef SK_DEBUG
	cerr << __FUNCTION__ << " : PWD -> " << curr_dir << endl ;
	#endif
	long long free_space = (long long) si.available ;
	#ifdef SK_DEBUG
	cerr << __FUNCTION__ << " : free_space -> " << free_space << endl ;
	#endif
	if ( free_space < 1 * 1024 * 1024 )
	{
		perror("Not enough free space. Shutting down.\n") ;
		sys_poweroff() ;
		return 0 ;
	}
	else return 1 ;
}
unsigned long long int timenow()
  {
	return ((std::chrono::duration_cast<std::chrono::milliseconds> 
	((std::chrono::time_point_cast<std::chrono::milliseconds>
	(std::chrono::system_clock::now())).time_since_epoch())).count()) ;
}
  
  double find_optimum_exposure ( unsigned short * picdata , unsigned int imgsize , double exposure )
{
	//#define SK_DEBUG
	#ifdef SK_DEBUG
	cerr << __FUNCTION__ << " : Received exposure: " << exposure << endl ;
	#endif
	double result = exposure ;
	double val ;
	qsort(picdata,imgsize,sizeof(unsigned short),compare) ;

	#ifdef MEDIAN
	if ( imgsize && 0x01 )
		val = ( picdata[imgsize/2] + picdata[imgsize/2+1] ) * 0.5 ;
	else
		val = picdata[imgsize/2] ;
	#endif //MEDIAN

	#ifndef MEDIAN
	#ifndef PERCENTILE
	#define PERCENTILE 90.0
	bool direction ;
	if ( picdata[0] < picdata[imgsize-1] )
		direction = true ;
	else
		direction = false ;

	unsigned int coord = floor((PERCENTILE*(imgsize-1)/100.0)) ;
	if ( direction )
		val = picdata[coord] ;
	else
	{
		if ( coord == 0 )
			coord = 1 ;
		val = picdata[imgsize-coord] ;
	}

	#ifdef SK_DEBUG
	cerr << "Info: " << __FUNCTION__ << "Direction: " << direction << ", Coordinate: " << coord << endl ;
	cerr << "Info: " << __FUNCTION__ << "10 values around the coordinate: " << endl ;
	unsigned int lim2 = imgsize - coord > 3 ? coord + 4 : imgsize - 1 ;
	unsigned int lim1 = lim2 - 10 ;
	for ( int i = lim1 ; i < lim2 ; i++ )
		cerr << picdata[i] << " ";
	cerr << endl ;
	#endif

	#endif //PERCENTILE
	#endif //MEDIAN
	#ifdef SK_DEBUG
	cerr << "In " << __FUNCTION__ << ": Median: " << val << endl ;
	#endif
	#ifndef PIX_MEDIAN
	#define PIX_MEDIAN 40000.0
	#endif

	#ifndef PIX_GIVE
	#define PIX_GIVE 5000.0
	#endif

	if ( val > PIX_MEDIAN - PIX_GIVE && val < PIX_MEDIAN + PIX_GIVE /* && PIX_MEDIAN - PIX_GIVE > 0 && PIX_MEDIAN + PIX_GIVE < 65535 */ )
		return result ;

	/** If calculated median pixel is within PIX_MEDIAN +/- PIX_GIVE, return current exposure **/

	result = ((double)PIX_MEDIAN) * exposure / ((double)val) ;

	#ifdef SK_DEBUG
	cerr << __FUNCTION__ << " : Determined exposure from median " << val << ": " << result << endl ;
	#endif
    
    while ( result > MAX_ALLOWED_EXPOSURE && pix_bin < 4 )
    {
        result /= 4 ;
        pix_bin *= 2 ;
		#ifdef SK_DEBUG
		cerr << "exposure: " << result << " bin: " << pix_bin << endl;  
		#endif
    }
    
    while ( result <= maxShortExposure && pix_bin > 1 )
    {
        result *= 4 ;
        pix_bin /= 2 ;
		#ifdef SK_DEBUG
		cerr << "exposure: " << result << " bin: " << pix_bin << endl;  
		#endif
    }
	if ( result <= minShortExposure )
		return minShortExposure ;
	unsigned long mult = floor ( result / minShortExposure ) ;
	result = mult * minShortExposure ;
    if ( pix_bin < 0 )
        pix_bin = 1 ;
    if ( pix_bin > 4 )
        pix_bin = 4 ;
	return result ;
	//#undef SK_DEBUG
}
  
bool snap_picture ( AtikCamera * device , unsigned pixX , unsigned pixY , unsigned short * data , double exposure  )
{
	cerr << "Camera Thread: Snapping image, " << exposure << "s exposure" << 
	" Bin: " << pix_bin << endl ;
	bool success ;
	if ( exposure <= maxShortExposure )
	{
		#ifdef SK_DEBUG
		cerr << "Info: Exposure time less than max short exposure, opting for short exposure mode." << endl ;
		#endif
		success = device -> readCCD(0,0,pixX,pixY,pix_bin,pix_bin,exposure) ;
		if ( ! success )
		{	
			cerr << "Error: Short exposure failed." << endl ;
			return success ;
		}
	}
	else if ( exposure > maxShortExposure )
	{
		#ifdef SK_DEBUG
		cerr << "Info: Exposure time greater than max short exposure, opting for long exposure mode." << endl ;
		#endif
		success = device ->startExposure(false) ; //false for some gain mode thing
		if ( ! success )
		{	
			cerr << "Error: Failed to start long exposure." << endl ;
			return success ;
		}
		long delay = device -> delay(exposure) ;
		#ifdef SK_DEBUG
		cerr << "Info: Long exposure delay set to " << delay << " ms." << endl ;
		#endif
		usleep(delay) ;
		success = device -> readCCD(0,0,pixX,pixY,pix_bin,pix_bin) ;
		if ( ! success )
		{	
			cerr << "Error: Failed to stop long exposure." << endl ;
			return success ;
		}

	}
	else return false ;
	unsigned width = device -> imageWidth(pixX,pix_bin) ;
	unsigned height = device -> imageHeight(pixY,pix_bin) ;
	success = device -> getImage(data,width*height) ;
	return success ;
}

//Main routine
void * camera_thread(void *t)
{
    /** Atik Camera Temperature Log **/
	#ifndef TEMPLOG_LOCATION
	#define TEMPLOG_LOCATION "/home/pi/temp_log.bin"
	#endif

	templog.open( TEMPLOG_LOCATION , ios::binary | ios::app ) ;
	bool templogstat = true ;
	if ( !templog.good() )
	{
		cerr << "Error: Unable to open temperature log stream." << endl ;
		templogstat = false ;
	}
	#ifdef SK_DEBUG
	cerr << "Info: Opened temperature log file" << endl ;
	#endif
	/*********************************/

	/** Camera Missing Log **/
	ofstream camlog ;
	#ifndef CAMLOG_LOCATION
	#define CAMLOG_LOCATION "/home/pi/cam_log.bin"
	#endif

	camlog.open(CAMLOG_LOCATION,ios::binary | ios::app) ;
	if ( !camlog.good() )
	{
		cerr << "Error: Unable to open camera log stream." << endl ;
	}
	#ifdef SK_DEBUG
	cerr << "Info: Opened camera log file." << endl ;
	#endif
	/************************/

	unsigned char firstrun = 1 ;
	sleep(5); //initially sleep for 5 to get everything up and running
	do {
		if ( ! firstrun ){
			cerr << "Camera not found. Waiting " << TIME_WAIT_USB / 1000000 << " s..." << endl ;
			usleep ( TIME_WAIT_USB ) ; //spend 1 seconds between looking for the camera every subsequent runs
			// #ifdef RPI
			// if ( cam_off )
			// {
			// 	usleep ( 1000000 * 60 ) ;
			// 	cam_off = false ;
			// 	gpioWrite(17,1) ;
			// }
			// #endif
		}
		int count = AtikCamera::list(devices,MAX) ;
		cerr << "List: " << count << " number of devices." << endl ;
		if ( ! count )
		{
			put_data(camlog, timenow()); camlog << (unsigned char) 0x00 ;
		}
		volatile bool success1 , success2 ; //two values used by two threads
		while ( count-- ) //loop so that I can break whenever ANY error is detected (probably means camera disconnected)
		{
			AtikCamera * device = devices[0] ; //first camera is our camera

			success1 = device -> open() ; //open device for access
			cerr << "Info: device -> open(): " << success1 << endl ;

			if ( !success1 ){ //if failed
				cerr << __FILE__ << ":" << __LINE__ << ":device->open()" << endl ;
				errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": Error: Failed to open device for first time." << endl ;
				break ; //get out and fall back to the main loop
			}

			cerr << "Name: " << device -> getName() << endl ;

			AtikCapabilities * devcap = new AtikCapabilities ;
			const char * devname ; CAMERA_TYPE type ;

			success1 = device -> getCapabilities(&devname, &type, devcap) ;
			#ifdef SK_DEBUG
			cerr << "Info: getCapabilities: " << success1 << endl ;
			#endif
			if ( !success1 ){ //if failed
				cerr << __FILE__ << ":" << __LINE__ << ":device->getCapabilities()" << endl ;
				errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": Error: Failed to get device capabilities." << endl ;
				device -> close() ;
				break ; //get out and fall back to the main loop
			}
			else {
				#ifdef SK_DEBUG
				cerr << "Device: " << "Returned Capabilities" << endl ;
				#endif
			}

			unsigned       pixelCX = devcap -> pixelCountX ;
			unsigned       pixelCY = devcap -> pixelCountY ;

			double         pixelSX = devcap -> pixelSizeX ;
			double         pixelSY = devcap -> pixelSizeY ;

			unsigned       maxBinX = devcap -> maxBinX ;
			unsigned       maxBinY = devcap -> maxBinY ;

			unsigned tempSensCount = devcap -> tempSensorCount ;

			int            offsetX = devcap -> offsetX ;
			int            offsetY = devcap -> offsetY ;

			bool       longExpMode = devcap -> supportsLongExposure ;

			           minShortExposure = devcap -> minShortExposure ;
			           maxShortExposure = devcap -> maxShortExposure ;

			if ( pix_bin > maxBinX || pix_bin > maxBinY )
				pix_bin = maxBinX < maxBinY ? maxBinX : maxBinY ; //smaller of the two 

			unsigned       width  = device -> imageWidth(pixelCX, pix_bin) ;
			unsigned       height = device -> imageHeight(pixelCY, pix_bin) ; 
			#ifdef SK_DEBUG
			cerr << "Device: AtikCapabilities:" << endl ;
			cerr << "Pixel Count X: " << pixelCX << "; Pixel Count Y: " << pixelCY << endl ;
			cerr << "Pixel Size X: " << pixelSX << " um; Pixel Size Y: " << pixelSY << " um" << endl ;
			cerr << "Max Bin X: " << maxBinX << "; Max Bin Y: " << maxBinY << endl ;
			cerr << "Temperature Sensors: " << tempSensCount << endl ;
			cerr << "Offset X: " << offsetX << "; Offset Y: " << offsetY << endl ;
			cerr << "Long Exposure Mode Supported: " << longExpMode << endl ;
			cerr << "Minimum Short Exposure: " << minShortExposure << " ms" << endl ;
			cerr << "Maximum Short Exposure: " << maxShortExposure << " ms" << endl ;
			cerr << "Binned X width: " << width << endl ;
			cerr << "Binned Y height: " << height << endl ;
			#endif
			if ( minShortExposure > maxShortExposure )
			{
				#ifdef SK_DEBUG
				cerr << "Error: Minimum short exposure > Maximum short exposure. Something wrong with camera. Breaking and resetting." << endl ;
				#endif
				errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "Error: Minimum short exposure > Maximum short exposure. Something wrong with camera. Breaking and resetting." << endl ;
				delete devcap ;
				device -> close() ;
				break ;
			}

			double exposure = maxShortExposure ; // prepare for first exposure to set baseline

			unsigned short * picdata = NULL ;

			unsigned imgsize = width * height ;

			picdata   = new unsigned short[imgsize] ;

			if ( picdata == NULL )
			{
				cerr << "Fatal Error: Could not allocate memory for picture buffer." << endl ;
				errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": Fatal Error: Failed to allocate memory for pixel data. Rebooting..." << endl ;
				sys_reboot() ;
				pthread_exit(NULL);
			}

			success2 = true ; //just to initiate the for loop...
            float temp ;
			if ( templogstat )
			{
				for ( unsigned sensor = 1 ; success2 && sensor <= tempSensCount ; sensor ++ )
				{
					success2 = device -> getTemperatureSensorStatus(sensor, &temp) ;
					if ( temp > 40.0 )
						raise(SIGILL);
					templog << (unsigned char) sensor ;
					put_data(templog, timenow());
					put_data(templog, temp);
					templog << (unsigned char) 0x00 ;
					#ifdef SK_DEBUG
					cerr << "Info: Sensor " << sensor << ": Temp: " << temp << " C" << endl ;
					#endif
				}	
			}

			/** Take the first picture! **/
			#ifdef SK_DEBUG
			cerr << "Info: Preparing to take first exposure." << endl ;
			#endif
			unsigned long long int tnow = timenow() ; //measured time
			success1 = device -> readCCD ( 0 , 0 , pixelCX , pixelCY , pix_bin , pix_bin , maxShortExposure ) ;
			#ifdef SK_DEBUG
			cerr << "Info: Exposure Complete. Returned: " << success1 << endl ;
			#endif
			if ( ! success1 )
			{
				#ifdef SK_DEBUG
				cerr << "Error: Could not complete first exposure. Falling back to loop 1." << endl ;
				#endif
				errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "Error: Could not complete first exposure. Falling back to loop 1." << endl ;
				delete [] picdata ;
				delete devcap ;
				device -> close() ;
				break ;
			}
			success1 = device -> getImage ( picdata , imgsize ) ;
			if ( ! success1 )
			{
				#ifdef SK_DEBUG
				cerr << "Error: Could not get data off of the camera. Falling back to loop 1." << endl ;
				#endif
				errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "Error: Could not get data off of the camera. Falling back to loop 1." << endl ;
				delete [] picdata ;
				delete devcap  ;
				device -> close() ;
				break ;
			}
			#ifdef SK_DEBUG
			cerr << "Info: getImage() -> " << success1 << endl ;
			#endif

			/** Let's save the data first **/
			string gfname ;
			gfname = to_string(tnow) + ".fit[compress]" ;
			image * imgdata = new image ;
			
			imgdata -> tnow = tnow ;
			imgdata -> pixx = width ;
			imgdata -> pixy = height ;
            imgdata -> imgsize = width*height ;
			imgdata -> exposure = exposure ;
            memcpy(&(imgdata->picdata), picdata, width*height *sizeof(unsigned short));
			//(imgdata -> picdata) = picdata ; // FIX WITH MEMCPY

			if ( save(gfname.c_str(),imgdata) )
			{
				#ifdef SK_DEBUG
				cerr << "Error: Could not open filestream to write data to." << endl ;
				#endif
				errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "Error: Could not open output stream. Check for storage space?" << endl ;
				delete[] picdata ;
				delete devcap  ;
				device -> close() ;
				break ;
			}
			#ifdef SK_DEBUG
			cerr << "Info: Wrote tnow -> " << tnow << ", exposure -> " << exposure << endl ;
			#endif

			/*****************************/

			/** Exposure Determination Routine **/
			#ifdef SK_DEBUG
			cerr << "Info: Calculating new exposure." << endl ;
			cerr << "Old Exposure -> " << exposure << " ms," << endl ;
			#endif
			exposure = find_optimum_exposure(picdata, imgsize, exposure) ;
			#ifdef SK_DEBUG
			cerr << "New Exposure -> " << exposure << "ms." << endl ;
			#endif
			if ( exposure < 0 ) //too bright
			{
				#ifdef SK_DEBUG
				cerr << "OpticsError: Too bright surroundings. Exiting for now." << endl ;
				#endif
				errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "OpticsError: Too bright surroundings. Exiting for now." << endl ;
				delete[] picdata ;
				delete devcap ;
				device -> close() ;
				break ;
			}

			#ifdef SK_DEBUG
			cerr << "Loop conditions: done " << done << " : success1 " << success1 << " : space_left() " << space_left() << endl ;
			#endif

			/************************************/
			omp_set_num_threads( 2 ) ;
			success2 = true ;
			while ( ( ! done) && success1 && (space_left() > 0 ))
			{
				#ifdef SK_DEBUG
				cerr << "Info: Entered loop mode." << endl ;
				#endif
				double old_exposure = exposure ;
				/** Taking Picture and logging temperature **/
				if ( exposure <= maxShortExposure )
				{
					#ifdef SK_DEBUG
					cerr << "Info: Loop: Short exposure mode." << endl ;
					#endif
					tnow = timenow() ;
					success1 = snap_picture(device, pixelCX, pixelCY, picdata, exposure) ;
					#ifdef SK_DEBUG
					cerr << "Info: Loop: Short: " << tnow << " obtained image." << endl ;
					#endif
					if ( ! success1 )
					{
						#ifdef SK_DEBUG
						cerr << "[" << tnow << "] Error: Single Thread Mode: Could not snap picture." << endl ; 
						#endif
						errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "Error: Single Thread Mode: Could not snap picture." << endl ;
					}
					if ( templogstat )
					{
						for ( unsigned sensor = 1 ; success2 && sensor <= tempSensCount ; sensor ++ )
						{
							success2 = device -> getTemperatureSensorStatus (sensor, &temp) ;
							templog << (unsigned char) sensor ;
							put_data(templog, timenow());
							put_data(templog, temp);
							templog << (unsigned char) 0x00 ;
							if ( temp > 40.0 )
								raise(SIGILL);
							#ifdef SK_DEBUG
							cerr << "Info: Sensor: " << sensor << " Temp: " << temp << " C" << endl ;
							#endif
						}
						if ( ! success2 )
						{
							#ifdef SK_DEBUG
							cerr << "[" << timenow() << "]: Warning: Could not read temperature sensor." << endl ;
							#endif
							errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "Warning: Could not read temperature sensor." << endl ; 
						}
					}
				}
				else
				{
					#ifdef SK_DEBUG
					cerr << "Info: Loop: Long" << endl ;
					#endif
					volatile bool pic_taken = false ;
					#pragma omp parallel default(shared)//take pictures and temperature readings
					{
						if ( omp_get_thread_num( ) == 0 ) //first thread to take pictures
						{	
							tnow = timenow() ;
							success1 = snap_picture ( device, pixelCX, pixelCY, picdata, exposure ) ;
							#ifdef SK_DEBUG
							cerr << "Info: Loop: Long: " << tnow << " obtained image." << endl ;
							#endif
							pic_taken = true ;
							if ( ! success1 )
							{
								#ifdef SK_DEBUG
								cerr << "[" << tnow << "] Error: Could not snap picture." << endl ; 
								#endif
								errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "Error: Could not snap picture." << endl ;
							}
						}
						if ( omp_get_thread_num( ) == 1 ) //second thread to take temperature
						{
							while(!pic_taken)
							{
								if ( templogstat )
								{
									for ( unsigned sensor = 1 ; success2 && sensor <= tempSensCount ; sensor ++ )
									{
										temp ; success2 = device -> getTemperatureSensorStatus(sensor, &temp) ;
										if ( temp > 40 )
											raise(SIGILL);
										templog << (unsigned char) sensor ;
										put_data(templog, timenow());
										put_data(templog, temp);
										templog << (unsigned char) 0x00 ;
										#ifdef SK_DEBUG
										cerr << "Info: Sensor: " << sensor << " Temp: " << temp << " C" << endl ;
										#endif
									}
								}
								usleep ( 100000 ) ; //100ms
							}
							#ifdef SK_DEBUG
							cerr << "Info: Sensor: 1" << " Temp: " << temp << " C" << endl ;
							#endif
						}
					}
				}
				/** End Taking picture and logging temperature **/
				#ifdef SK_DEBUG
				cerr << "Info: Picture taken. Processing." << endl ;
				#endif
				/** Post-processing **/
				gfname = to_string(tnow) + ".fit[compress]" ;
				//image * imgdata = new image ;
                
                width  = device -> imageWidth(pixelCX, pix_bin) ;
                height = device -> imageHeight(pixelCY, pix_bin) ;
                
				imgsize = width * height ;
				cerr << "Width: " << width << " Height: " << height << endl ;

				imgdata -> tnow = tnow ;
				imgdata -> pixx = width ;
				imgdata -> pixy = height ;
                imgdata -> imgsize = width*height ;
				imgdata -> exposure = exposure ;
                imgdata -> ccdtemp = floor(temp*100) ;
                imgdata -> boardtemp = boardtemp ;
                imgdata -> chassistemp = chassistemp ;
                memcpy(&(imgdata->picdata), picdata, width*height *sizeof(unsigned short));
				
				#ifdef DATAVIS
				convert_to_packet(imgdata, &(global_p.a));
				// global_p.a.tnow = tnow ;
				// global_p.a.pixx = width ;
				// global_p.a.pixy = height ;
                // global_p.a.imgsize = width*height ;
				// global_p.a.exposure = exposure ;
                // global_p.a.ccdtemp = floor(temp*100) ;
                // global_p.a.boardtemp = boardtemp ;
                // global_p.a.chassistemp = chassistemp ;
				// cerr << "Camera: " << global_p.a.exposure << endl ;
				#endif
                if ( save(gfname.c_str(), imgdata) )
				{
					#ifdef SK_DEBUG
					cerr << "Error: Could not open filestream to write data to." << endl ;
					#endif
					errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "Error: Could not open output stream. Check for storage space?" << endl ;
					delete[] picdata ;
					delete devcap  ;
					device -> close() ;
					break ;
				}
				sync() ;
				
				#ifdef SK_DEBUG
				cerr << "Info: Loop: Wrote data to disk." << endl ;
				#endif
				/*****************************/

				/** Exposure Determination Routine **/
				#ifdef SK_DEBUG
				cerr << "Info: Loop: Old exposure: " << old_exposure << " s" << endl ;
				#endif
				exposure = find_optimum_exposure(picdata, imgsize, exposure) ;
				// for ( int i = 0 ; i < 1392*1040 ; i++ )
				// 	picdata[i] = 0 ;
				#ifdef SK_DEBUG
				cerr << "Info: Loop: New exposure: " << exposure << " s" << endl ;
				#endif
				if ( exposure < minShortExposure ) //too bright
				{
					#ifdef SK_DEBUG
					cerr << "OpticsError: Too bright surroundings. Setting up minimum exposure." << endl ;
					#endif
					errlog << "[" << timenow() << "]" << __FILE__ << ": " << __LINE__ << ": " << "OpticsError: Too bright surroundings. Setting minimum exposure." << endl ;
					// delete [] picdata ;
					// delete devcap ;
					// break ;
					exposure = minShortExposure ;
				}
				if ( old_exposure < PIC_TIME_GAP ) //sleep for rest of the time if on shorter than PIC_TIME_GAP (s) exposure
					usleep((long)(PIC_TIME_GAP-old_exposure)*1000000) ;
				/************************************/
				/*********************/

			} //loop 3
			device -> close() ;
			delete[] picdata ;
			delete imgdata ;
			delete devcap ;
		} //loop 2
		firstrun = 0 ;
	} while ( ! done ) ; //loop 1
    cerr << "Camera Thread: Received kill signal" << endl ;
    camlog .close() ;
	templog.close() ;
	errlog .close() ;
    pthread_exit(NULL) ;
}
