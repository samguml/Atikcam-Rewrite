  /**
@file datavis.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief main module header, contains all function prototypes of datavis module
@Date 9/10/2020
@copyright Copyright (c) 2020
**/

#ifndef DATAVIS_H
#define DATAVIS_H
#endif

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


//Function prototypes are as follows
#ifndef PACK_SIZE
#define PACK_SIZE sizeof(datavis_p)
#endif

void * datavis_thread(void *t)
  
