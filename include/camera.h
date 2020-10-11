/**
@file camera.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief main module header, contains all function prototypes of camera module
@Date 9/10/2020
@copyright Copyright (c) 2020
**/

#include <camera_extern.h>

#ifndef CAMERA_H
#define CAMERA_H
#endif
//Function prototypes are as follows

typedef struct image {
	uint64_t tnow ; 
	float exposure ;
	unsigned short pixx ; 
	unsigned short pixy ; 
    unsigned int imgsize ; 
    short ccdtemp ; 
    short boardtemp ;
    short chassistemp ;
	unsigned short picdata[1449072] ;
	unsigned char unused[6] ; 
	unsigned char unused2[1792] ; //padding to round off to 708*4096 bytes
} image ; //size 708*4096

void convert_to_packet(image * a , datavis_p * b)

inline void put_data ( ostream & str , unsigned short val )
inline void put_data ( ostream & str , unsigned long long int val )
inline void put_data ( ostream & str , float val )
inline void put_data ( ostream & str , char val ) 
  
int save(const char *fileName, image* data)
 
void sys_reboot(void)
 
double find_optimum_exposure ( unsigned short * picdata , unsigned int imgsize , double exposure )
  
bool snap_picture ( AtikCamera * device , unsigned pixX , unsigned pixY , unsigned short * data , double exposure  )
  
char space_left(void)

 
void * camera_thread(void *t)
  
  
