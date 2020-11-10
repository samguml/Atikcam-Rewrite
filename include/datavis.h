  /**
@file datavis.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief main module header, contains all function prototypes of datavis module
@Date 9/10/2020
@copyright Copyright (c) 2020
**/

#ifndef _DATAVIS_H
#define _DATAVIS_H

#include <datavis_extern.h>
#include <macros.h>
#include <unistd.h>
#include <sys/socket.h> 

//Macros for datavis thread
#ifndef CCD_COOLDOWN_TIME
#define CCD_COOLDOWN_TIME 60*1000 // in milliseconds
#endif

#ifndef PORT
#define PORT 12376
#endif //PORT

//Datavis structs



typedef struct sockaddr sk_sockaddr; 

//Function prototypes are as follows

void * datavis_thread(void *t);
  
#endif //_DATAVIS_H
