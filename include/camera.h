/**
@file camera.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief main module header, contains all function prototypes of camera module
@Date 9/10/2020
@copyright Copyright (c) 2020
**/


#ifndef _CAMERA_H
#define _CAMERA_H
#include <camera_extern.h>
#include <macros.h>
#include <iostream>
//#include <atikccdusb.h>

//Macros for CameraThread

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
//End of camera macros

//Function prototypes are as follows

void convert_to_packet(image * a , datavis_p * b);

inline void put_data ( ostream & str , unsigned short val );
inline void put_data ( ostream & str , unsigned long long int val );
inline void put_data ( ostream & str , float val );


int save(const char *fileName, image* data);

char space_left(void);
 
unsigned long long int timenow();

double find_optimum_exposure ( unsigned short * picdata , unsigned int imgsize , double exposure );

bool snap_picture ( AtikCamera * device , unsigned pixX , unsigned pixY , unsigned short * data , double exposure  );

void sys_reboot(void);

void * camera_thread(void *t);
  
  
#endif //_CAMERA_H
