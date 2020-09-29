/**
@file main.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief provides threat init, spawn, join, destroy. Signal handler for all threads. Would contain error printer but that feature will be phased out 
@Date 9/28/2020

**/
#include <main.h>
#include <module.c>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

//signal handling
 struct sigaction action[3] ;
	memset(&action[0], 0, sizeof(struct sigaction)) ;
	action[0].sa_handler = term ;
    if (sigaction(SIGTERM, &action[0],NULL )<0)
    {
        perror("Main: SIGTERM Handler") ;
        cerr << "Main: SIGTERM Handler failed to install" << endl ;
		if (errlog.good()) errlog << "Main: SIGTERM Handler failed to install, errno" << errno << endl ;
    }
	memset(&action[1], 0, sizeof(struct sigaction)) ;
	action[1].sa_handler = term ;
	if ( sigaction(SIGINT, &action[1],NULL ) < 0 )
    {
        perror("Main: SIGINT Handler") ;
        cerr << "Main: SIGINT Handler failed to install" << endl ;
		if (errlog.good()) errlog << "Main: SIGINT Handler failed to install, errno" << errno << endl ;
    }
    memset(&action[2], 0, sizeof(struct sigaction)) ;
	action[2].sa_handler = overheat ;
	if ( sigaction(SIGILL, &action[2],NULL ) < 0 )
    {
        perror("Main: SIGILL Handler") ;
        cerr << "Main: SIGILL Handler failed to install" << endl ;
		if (errlog.good()) errlog << "Main: SIGILL Handler failed to install, errno" << errno << endl ;
    }
    cerr << "Main: Interrupt handlers are set up." << endl ;
