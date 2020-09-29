/**
@file housekeeping.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief main module header, contains all function prototypes of housekeeping module
@Date 9/10/2020
@copyright Copyright (c) 2020
**/



/*
//Function prototypes are as follows

inline void put_data ( ostream & str , unsigned short val )
inline void put_data ( ostream & str , unsigned long long int val )
inline void put_data ( ostream & str , float val )
inline void put_data ( ostream & str , char val )

unsigned long long int timenow()
//again need to ask about this one
void * housekeeping_thread(void *t)
*/

  
 
 //Allocation of memory for cross-module vairables  
volatile int boardtemp;

volatile int chassistemp ;

volatile unsigned long long camofftime;

bool cam_off;

volatile bool ccdoverheat;


