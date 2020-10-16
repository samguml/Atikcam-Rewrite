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

typedef struct {
	uint64_t tnow ; //timestamp in ms
	float exposure ; //exposure in ms
	unsigned short pixx ; //348
	unsigned short pixy ; //260
	unsigned char pixbin ;
	short ccdtemp ; // temp in C * 100
	short boardtemp ;
	short chassistemp ;
	unsigned char picdata[90480];
} datavis_p ;

typedef struct sockaddr sk_sockaddr; 

//Function prototypes are as follows

void * datavis_thread(void *t);
  
#endif //_DATAVIS_H
