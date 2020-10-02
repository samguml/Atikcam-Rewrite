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
#include <datavis.h>
#include <pthread.h>


//registers exec functions of modules
void *module_exec[] = {

camera_thread
housekeeping_thread
datavis_thread
  
};

//gives # of enabled modules
const int num_systems = sizeof(module_exec) / sizeof(void *);


