/**
@file main.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief Includes all headers that flightcam_v2-edited.cpp needs to run
@Date 9/3/2020

@copyright Copyright (c) 2020

**/

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
