/*
@file camera_extern.h
@author: Samuel Gregory
@date 9/28/20
@brief: contains extern defnitons for shared variables and synchronizers between modules
*/ 

#include <camera.h>
#include <pthread.h>
#ifndef CAMERA_EXTERN_H
#define CAMERA_EXTERN_H
#endif

extern double  minshortexposure;

extern double maxshortexposure;

extern unsigned  pix_bin;
