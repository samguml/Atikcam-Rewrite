/*
 @file housekeeping_iface.h
 @author Samuel Gregory
 @brief This file contains the extern prototypes for module_init, module_exec, module_exit, and pthread_cond_t for the housekeeping thread
 @date 9/28/20
 
 
 */
#ifndef HOUSEKEEPING_IFACE_H
#define HOUSEKEEPING_IFACE_H
#include <housekeeping_extern.h>
 void *housekeeping_thread(void *);
#endif //HOUSEKEEPING_IFACE_H



