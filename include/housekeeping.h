/**
@file housekeeping.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief main module header, contains all function prototypes of housekeeping module
@Date 9/10/2020
@copyright Copyright (c) 2020
**/

#ifndef _HOUSEKEEPING_H
#define _HOUSEKEEPING_H
#include <housekeeping_extern.h>
#include <macros.h>
#include <iostream>
#include <string>//not sure if this is needed, but better safe than sorry

//Macros for housekeeping thread
#ifndef CCD_COOLDOWN_TIME
#define CCD_COOLDOWN_TIME 60*1000 // in milliseconds
#endif

//Function prototypes

typedef union flb { float f ; char b[sizeof(float)] ; } flb ;
typedef union shb { unsigned short s ; char b[sizeof(unsigned short)] ; } shb ;
typedef union llb { unsigned long long int l ; char b[sizeof(long)] ; } llb ;

unsigned long long int timenow();
 
inline void put_data ( ostream & str , unsigned short val ) ;
inline void put_data ( ostream & str , unsigned long long int val );
inline void put_data ( ostream & str , float val );
inline void put_data ( ostream & str , char val );

void * housekeeping_thread(void *t);

#endif //_HOUSEKEEPING_h
  



