/*
 @file camera_iface.h
 @author Samuel Gregory
 @brief: This file contains the extern prototypes for module_init, module_exec, module_exit, and pthread_cond_t
 @date 9/28/20
 
 
 */
#ifndef CAMERA_IFACE_H
#define CAMERA_IFACE_H
#include <camera_extern.h>
void *camera_thread(void *);
#endif 


