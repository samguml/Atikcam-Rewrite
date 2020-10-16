/**
@file modules.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief Includes the various module functions 
@Date 9/12/2020
**/

#include <stdio.h>

#ifndef MODULES_H
#define MODULES_H
#ifdef MAIN_PRIVATE
#include <housekeeping_iface.h>
#include <camera_iface.h>
#include <datavis_iface.h>
#include <pthread.h>


//Note: No module_exit or module_init functions were found 


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

//list of condition locks would go here, but this part of modules.h is still in development

#endif //#define MAIN_PRIVATE
#endif  //#define MODULES_H
