/**
@file housekeeping.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief main module header, contains all function prototypes of housekeeping module
@Date 9/10/2020
@copyright Copyright (c) 2020
**/

#include <housekeeping_extern.h>

#ifndef HOUSEKEEPING_H
#define HOUSEKEEPING_H
#endif


//Function prototypes

unsigned long long int timenow();
 
inline void put_data ( ostream & str , unsigned short val ) ;
inline void put_data ( ostream & str , unsigned long long int val );
inline void put_data ( ostream & str , float val );
 inline void put_data ( ostream & str , char val );

void * housekeeping_thread(void *t);


  



