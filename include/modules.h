/**
@file modules.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief Includes the various module functions 
@Date 9/12/2020
**/

#ifndef _MODULES_H
#define _MODULES_H
#ifdef MAIN_PRIVATE
#include <housekeeping_iface.h>
#include <camera_iface.h>
#include <datavis_iface.h>



//Note: No module_exit or module_init functions are present


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

//list of condition locks would go here, but no conditional wake-ups are in use by the program

#endif //#define _MAIN_PRIVATE
#endif  //#define _MODULES_H
