/**
@file main.h
@Author: Samuel Gregory 
@brief Holds extern definitions of done & thread local errno. Would contain error printer prototype, but that feature has been phased out.
@Date 9/28/2020
**/
//Note all modules include main.h in their .c file

#ifndef MAIN_H
#define MAIN_H


#include <signal.h>
#include <fstream.h>//


//extern definition of done (controls loops for all threads)
extern volatile sig_atomic_t done;

//extern definition of thread local errno/custom 
extern ofstream templog ;
extern ofstream camlog ;
extern ofstream errlog ;

//extern defintion of local signal handler? 
extern overheat(int signum);

//interrupt handler for sigint
  void catch_sigint(int sig);

//function prototypes from main.c

void sys_poweroff(void);

#endif //MAIN_H
