/*
@file camera_extern.h
@author: Samuel Gregory
@date 9/28/20
@brief: contains extern defnitons for shared variables and synchronizers between modules
*/ 
#ifndef CAMERA_EXTERN_H
#define CAMERA_EXTERN_H
#include <camera.h>  //This may not be necessary, but is included just in case. May be subject to change 



extern double  minshortexposure;

extern double maxshortexposure;

extern unsigned  pix_bin;

#endif //CAMERA_EXTERN_H
