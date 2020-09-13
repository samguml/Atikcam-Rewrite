/**
@file camera.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief main module header, contains all function prototypes of camera module
@Date 9/10/2020
@copyright Copyright (c) 2020
**/

//Function prototypes are as follows

void convert_to_packet(image * a , datavis_p * b) 
inline void put_data ( ostream & str , unsigned short val )
inline void put_data ( ostream & str , unsigned long long int val )
inline void put_data ( ostream & str , float val )
inline void put_data ( ostream & str , char val )
  
int save(const char *fileName , image * data)
  
void sys_reboot(void)
  
char space_left(void)
  
unsigned long long int timenow()
  
double find_optimum_exposure ( unsigned short * picdata , unsigned int imgsize , double exposure )
  
bool snap_picture ( AtikCamera * device , unsigned pixX , unsigned pixY , unsigned short * data , double exposure  )
  
void * camera_thread(void *t)
  
  
