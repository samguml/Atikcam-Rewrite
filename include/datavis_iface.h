/*
 @file datavis_iface.h
 @author Samuel Gregory
 @brief This file contains the extern prototypes for module_init, module_exec, module_exit, and pthread_cond_t for the datavis thread
 @date 10/1/20
 
 
 */
#ifndef DATAVIS_IFACE_H
#define DATAVIS_IFACE_H
#include <datavis_extern.h>
 void *datavis_thread(void *);
#endif 

