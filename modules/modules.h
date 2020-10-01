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

typedef int (*init_func)(void);     // typedef to create array of init functions
typedef void (*destroy_func)(void); // typedef to create array of destroy functions
//Register modules with init function

init_func module_init[] = {};

//Number of modules with init functions

const int num_init = sizeof(module_init) / sizeof(init_func);
//
destroy_func module_destroy[] = {};
//gives # of modules with a destroy function 

const int num_destroy = sizeof(module_destroy) / sizeof(destroy_func);
//registers exec functions of modules
void *module_exec[] = {};

//gives # of enabled modules
const int num_systems = sizeof(module_exec) / sizeof(void *);

//
