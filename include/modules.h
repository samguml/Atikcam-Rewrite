/**
@file modules.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief 
@Date 9/12/2020
@copyright Copyright (c) 2020
**/

#include <stdio.h>
#include <housekeeping_iface.h>
#include <camera_iface.h>
#include <pthread.h>


//registers exec functions of modules
void *module_exec[] = {
#ifdef CAMERA
    ,
camera_thread
#endif
      
#ifdef HOUSEKEEPING
    ,
housekeeping_thread
 #endif
#ifdef DATAVIS
    ,
datavis_thread
#endif  
};

//gives # of enabled modules
const int num_systems = sizeof(module_exec) / sizeof(void *);

//list of condition locks would go here, but I still need clarification on them

