/**
@file modules.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief 
@Date 9/12/2020
@copyright Copyright (c) 2020
**/

# include <stdio.h>
#include <housekeeping_iface.h>
#include <camera_iface.h>
#include <pthread.h>

typedef int (*init_func)(void);     // typedef to create array of init functions
typedef void (*destroy_func)(void); // typedef to create array of destroy functions
//Register modules with init function

init_func module_init[] = {};

//Number of modules with init functions

const int num_init = sizeof(module_init) / sizeof(init_func);
//Register modules with exit function

exit_func module_exit[] = {};
//Number of modules with exit functions
const int num_exit = sizeof(module_exit) / sizeof(exit_func);

//Register modules with exec functions 
//slightly unsure of the DATAVIS parts
void *module_exec[] = {
    
#ifdef DATAVIS
    ,
    datavis_thread
#endif
#ifdef SITL
    ,
    sitl_comm
#endif
};
//Number of modules with exec functions

const int num_systems = sizeof(module_exec) / sizeof(void *);

//Allows for modules to be woken up

pthread_cond_t *wakeups[] = {
#ifdef SITL
    &data_available
#endif 
#ifdef DATAVIS
    ,
    &datavis_drdy
#endif 
};
