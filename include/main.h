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

//Extern defintions for logs 
extern ofstream templog ;
extern ofstream camlog ;
extern ofstream errlog ;

//Declaration of num_wakeups (an array used in the signal handler)
const int num_wakeups = sizeof(wakeups) / sizeof(pthread_cond_t *);

//extern definition of done (controls loops for all threads)
extern volatile sig_atomic_t done;

//extern definition of thread local errno/custom 
extern ofstream templog ;
extern ofstream camlog ;
extern ofstream errlog ;

//extern defintion of local signal handler? 
extern overheat(int signum);


// interrupt handler for SIGINT
void catch_sigint(int);

//function prototypes from main.c

void sys_poweroff(void);

#endif //MAIN_H
