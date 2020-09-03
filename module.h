/**
@file main.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief Includes all headers that flightcam_v2-edited.cpp needs to run
@Date 9/3/2020

@copyright Copyright (c) 2020

**/
#include <module_iface.c>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <chrono>
#include <thread>
#include <pthread.h> //posix threads for camera control and temperature monitoring
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <sys/statvfs.h>
#include <limits.h>
#include <omp.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <fitsio.h> //CFITSIO for saving files

#include <boost/filesystem.hpp> //boost library for filesystem support
//using namespace boost::filesystem ;
using namespace std ;
#include <atikccdusb.h>

#ifdef RPI //building on the PI
#include <pigpio.h>
#endif

#include <libmcp9808.h>
#include <libads1115.h>

//Register modules with init function

init_func module_init[] = {};

//Number of modules with init functions

const int num_init = sizeof(module_init) / sizeof(init_func);
//Register modules with exit function

exit_func module_exit[] = {};
//Number of modules with exit functions
const int num_exit = sizeof(module_exit) / sizeof(exit_func);

//Register modules with exec functions 

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

//Number ofmoduelswith wake-up functions
const int num_wakeups = sizeof(wakeups) / sizeof(pthread_cond_t *);
#endif
#endif
