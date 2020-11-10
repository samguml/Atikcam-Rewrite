/*
@file macros.h
@author: Samuel Gregory
@datee: 10/15/20
@brief: contains the functions needed throughout the flightcam software 
*/ 


#ifndef _MACROS_H
#define _MACROS_H

//#include <fitsio.h>
#include <main.h>//main.h holds extern definitons for logs
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace std;


typedef union flb { float f ; char b[sizeof(float)] ; } flb ;
typedef union shb { unsigned short s ; char b[sizeof(unsigned short)] ; } shb ;
typedef union llb { unsigned long long int l ; char b[sizeof(long)] ; } llb ;

/* Internal data structure */
typedef struct image {
	uint64_t tnow ; // timestamp in ms (from epoch, in local time)
	float exposure ; //exposure length in seconds
	unsigned short pixx ; //pixel x
	unsigned short pixy ; //pixel y
    unsigned int imgsize ; //pixx*pixy
    short ccdtemp ; // temp in C = temp/100
    short boardtemp ;
    short chassistemp ;
	unsigned short picdata[1449072] ;
	unsigned char unused[6] ; //first set of padding, default for GCC -> multiple of 32
	unsigned char unused2[1792] ; //padding to round off to 708*4096 bytes
} image ; //size 708*4096
/* End internal data structure */

typedef struct {
	uint64_t tnow ; //timestamp in ms
	float exposure ; //exposure in ms
	unsigned short pixx ; //348
	unsigned short pixy ; //260
	unsigned char pixbin ;
	short ccdtemp ; // temp in C * 100
	short boardtemp ;
	short chassistemp ;
	unsigned char picdata[90480];
} datavis_p ;

#ifndef PACK_SIZE
#define PACK_SIZE sizeof(datavis_p)
#endif



typedef union{
	datavis_p a ;
	unsigned char buf [sizeof(datavis_p) / PACK_SIZE] [PACK_SIZE];
} packetize ;
packetize global_p ;

//Saving FITS image
#ifndef NOSAVEFITS
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
  else {
	  cerr << "Error: " << __FUNCTION__ << " : Could not save image." << endl ;
	  errlog << "Error: " << __FUNCTION__ << " : Could not save image." << endl ;
  }
  return status ;
}
#else
int save(const char *fileName, image* data)
{
	return false;
}
#endif



unsigned long long int timenow()
{
	return ((std::chrono::duration_cast<std::chrono::milliseconds> 
	((std::chrono::time_point_cast<std::chrono::milliseconds>
	(std::chrono::system_clock::now())).time_since_epoch())).count()) ;
}


//Fuction for shutting down and rebooting 
#ifdef ENABLE_PWOFF
void sys_poweroff(void)
{
	cerr << "Info: Poweroff instruction received!" << endl ;
	sync() ;
	setuid(0) ;
	reboot(LINUX_REBOOT_CMD_POWER_OFF) ;
}
#else
void sys_poweroff(void)
{	
	cerr << "Info: Poweroff instruction received!" << endl ;
	exit(0);
}
#endif

//Function for rebooting
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

#endif //_MACROS_H
