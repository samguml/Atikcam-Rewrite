/**
@file main.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief 
@Date 9/3/2020

@copyright Copyright (c) 2020

**/
#include <main.h>
#include <module.c>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

static AtikCamera *devices[MAX] ;
bool gpio_status;
volatile sig_atomic_t done = 0 ; //global interrupt handler
volatile bool ccdoverheat = false ; //global ccd temperature status
volatile unsigned long long camofftime = INFINITY ;
double minShortExposure = -1 ;
double maxShortExposure = -1 ;



    /* Set up interrupt handler */
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

// module_init, module_exec, module_cleanup

  for (int i = 0; i < num_init; i++)
    {
        int val = module_init[i]();
        if (val < 0)
        {
            sherror("Error in initialization!");
            exit(-1);
        }
    }
    printf("Done init modules\n");
    int rc[num_systems];                                         // fork-join return codes
    pthread_t thread[num_systems];                               // thread containers
    pthread_attr_t attr;                                         // thread attribute
    int args[num_systems];                                       // thread arguments (thread id in this case, but can be expanded by passing structs etc)
    void *status;                                                // thread return value
    pthread_attr_init(&attr);                                    // initialize attribute
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); // create threads to be joinable

    for (int i = 0; i < num_systems; i++)
    {
        args[i] = i; // sending a pointer to i to every thread may end up with duplicate thread ids because of access times
        rc[i] = pthread_create(&thread[i], &attr, module_exec[i], (void *)(&args[i]));
        if (rc[i])
        {
            printf("[Main] Error: Unable to create thread %d: Errno %d\n", i, errno);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr); // destroy the attribute

    for (int i = 0; i < num_systems; i++)
    {
        rc[i] = pthread_join(thread[i], &status);
        if (rc[i])
        {
            printf("[Main] Error: Unable to join thread %d: Errno %d\n", i, errno);
            exit(-1);
        }
    }

    // destroy modules
    for (int i = 0; i < num_destroy; i++)
    {
        module_destroy[i]();
    }
    return 0;
}


//Error handler

void sherror(const char *msg)
{
    switch (sys_status)
    {
    case ERROR_MALLOC:
        
        break;

    case ERROR_HBRIDGE_INIT:
        
        break;

    case ERROR_MUX_INIT:
        
        break;

    case ERROR_CSS_INIT:
        
        break;

    case ERROR_FSS_INIT:
        
        break;

    case ERROR_FSS_CONFIG:
      
        break;

	    default:
        break;
    }
}
