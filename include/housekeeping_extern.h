/*
@file camera_extern.h
@author: 9/28/20
@brief: contains extern definitons for shared variables and synchronizers between modules
*/ 

//#include <housekeeping.h>
#include <pthread.h>

extern volatile int boardtemp;
extern volatile int chassistemp;
extern bool cam_off;
extern volatile unsigned long long camofftime:
extern volatile bool ccdoverheat;

