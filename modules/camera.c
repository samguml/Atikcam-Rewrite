/**
@file camera.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief
@Date 9/10/2020
@copyright Copyright (c) 2020
**/
#include  <housekeeping_extern.h>

//function definitions
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
inline void put_data ( ostream & str , char val )
 {
	 str << val ;
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
  
  void sys_reboot(void)
{
	cerr << "Info: Reboot instruction received!" << endl ;
	sync() ;
	setuid(0) ;
	reboot(RB_AUTOBOOT) ;
}
  
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

//ask about this
void * camera_thread(void *t)
  
