/*
@file camera_extern.h
@author: Samuel Gregory
@Date: 9/28/20
@brief: contains extern definitons for shared variables and synchronizers between modules
*/ 
#ifndef HOUSEKEEPING_EXTERN_H
#define HOUSEKEEPING_EXTERN_H
//Extern definitons for cross module variables-Note shares variables with camera thread but not with datavis thread

//extern defintions for shared variables 

extern volatile int boardtemp;

extern volatile int chassistemp;

extern bool cam_off;

extern volatile unsigned long long camofftime:

extern volatile bool ccdoverheat;

#endif //HOUSEKEEPING_EXTERN_H
