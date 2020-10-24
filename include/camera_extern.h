/*
@file camera_extern.h
@author: Samuel Gregory
@date 9/28/20
@brief: contains extern defnitons for shared variables and synchronizers between modules
*/ 
#ifndef CAMERA_EXTERN_H
#define CAMERA_EXTERN_H



//Extern definitons for cross module variables-Note shares variables with housekeeping thread but not with datavis thread
extern double  minshortexposure;

extern double maxshortexposure;

extern unsigned  pix_bin;

#endif //CAMERA_EXTERN_H
