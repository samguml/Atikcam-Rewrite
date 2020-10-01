/**
@file main.h
@Author: Samuel Gregory 
@brief has source code for every module. Also holds extern definitions. Would contain error printer prototype, but that feature will not be included.
@Date 9/28/2020
**/
//Note all modules include main.h in their .c file!

#ifndef MAIN_H
#define MAin_H
#endif

//extern definition of done (controls loops for all threads)
extern volatile sig_atomic_t done = 0 ;

//extern definition of thread local errno/custom 
extern ofstream templog ;
extern ofstream camlog ;
extern ofstream errlog 

//extern defintion of local signal handler? 
extern overheat(int signum)


//Signal Handler prototype- i think
void term (int signum);
