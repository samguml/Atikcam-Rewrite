/*
@file macros.h
@author: Samuel Gregory
@datee: 10/15/20
@brief: contains the funtions needed throughout the flightcam software 
*/ 

//Note:this software is still subject to change-macros.h is one of the files not mentioned in the desgin pdf

#include <stdio.h>

#ifndef _MACROS_H
#define _MACROS_H
#endif


//Global Macros
#define MAX 1

#ifndef MAX_ALLOWED_EXPOSURE
#define MAX_ALLOWED_EXPOSURE 40
#endif

#ifndef TIME_WAIT_USB
#define TIME_WAIT_USB 1000000
#endif

#ifndef PIC_TIME_GAP
#define PIC_TIME_GAP 10.0 // minimum gap between images in seconds
#endif

#ifndef PIX_MEDIAN
#define PIX_MEDIAN 45000.0
#endif

#ifndef PIX_GIVE
#define PIX_GIVE 5000.0
#endif

#ifndef PIX_BIN
#define PIX_BIN 1
#endif

#ifndef CCD_COOLDOWN_TIME
#define CCD_COOLDOWN_TIME 60*1000 // in milliseconds
#endif

#ifndef PORT
#define PORT 12376
#endif //PORT

#ifndef SERVER_IP
#define SERVER_IP "192.168.1.2"
#endif //SERVER_IP
//End of global macros

#ifndef PACK_SIZE
#define PACK_SIZE sizeof(datavis_p)
#endif

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


