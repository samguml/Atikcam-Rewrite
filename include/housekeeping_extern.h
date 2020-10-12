/*
@file camera_extern.h
@author: Samuel Gregory
@Date: 9/28/20
@brief: contains extern definitons for shared variables and synchronizers between modules
*/ 

#include <housekeeping.h> //This may not be necessary, but is included just in case. May be subject to change 
#include <pthread.h>
#ifndef HOUSEKEEPING_EXTERN_H
#define HOUSEKEEPING_EXTERN_H
#endif

extern volatile int boardtemp;

extern volatile int chassistemp;

extern bool cam_off;

extern volatile unsigned long long camofftime:

extern volatile bool ccdoverheat;

