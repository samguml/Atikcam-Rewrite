/**
@file main.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief provides threat init, spawn, join, destroy. Signal handler for all threads. Would contain error printer but that feature will be phased out 
@Date 9/28/2020

**/
#include <main.h>
#include <modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>


bool gpio_status;
char curr_dir[PATH_MAX];
volatile sig_atomic_t done = 0;

int main ( void )
{
	/** Error Log **/
	ofstream errlog ;
	#ifndef ERRLOG_LOCATION
	#define ERRLOG_LOCATION "/home/pi/err_log.txt"
	#endif

	errlog.open(ERRLOG_LOCATION,ios::app) ;
	if(!errlog.good())
	{
		cerr << "Error: Unable to open error log stream." << endl ;
	}
    /***************/
    /* Setup GPIO */
    gpio_status = false;
    #ifdef RPI
    if ( gpioInitialise() < 0 ){
        perror("Main: GPIO Init") ;
        cerr << "Warning: pigpio init failed." << endl ;
    }
    else {
        gpio_status = true ;
        gpioSetMode(17,PI_OUTPUT) ; //12V Rail
        gpioSetMode(27,PI_OUTPUT) ; //28V Rail
    }
    #endif //RPI
    /* Set up interrupt handler Note: This handler comes from shflight and replaced the one that was in use. It has not been fully integrated as of yet but will be*/
    struct sigaction saction;
    saction.sa_handler = &catch_sigint;
    sigaction(SIGINT, &saction, NULL);
    // initialize modules
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
    // set up threads
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
/**
 * @brief SIGINT handler, sets the global variable `done` as 1, so that thread loops can break.
 
 * 
 * @param sig Receives the signal as input.
 */
void catch_sigint(int sig)
{
    done = 1;
    for (int i = 0; i < num_wakeups; i++)
        pthread_cond_broadcast(wakeups[i]);
}
    /* End set up interrupt handler */

    /* Look for free space at init */
    if ( getcwd(curr_dir,sizeof(curr_dir)) == NULL ) //can't get pwd? Something is seriously wrong. System is shutting down.
	{
		perror("Main: getcwd() error, shutting down.") ;
		if (errlog.good()) errlog << "Main: getcwd() error, shutting down. errno" << errno << endl ;
		sys_poweroff() ;
		return 1 ;
	}
    cerr << "Main: PWD: " << curr_dir << endl ;
    boost::filesystem::space_info si = boost::filesystem::space(curr_dir) ;
	long long free_space = (long long) si.available ;
	if ( free_space < 1 * 1024 * 1024 )
	{
		perror("Main: Not enough free space. Shutting down.") ;
		sys_poweroff() ;
		return 1 ;
	}
    /* End look for free space at init */

    /* Set up atik camera debug */
    #ifdef ATIK_DEBUG
    AtikDebug = true ;
    #else
    AtikDebug = false ;
    #endif //ATIK DEBUG

    int rc0, rc1, rc2 ;
    pthread_t thread0 , thread1, thread2 ;
    pthread_attr_t attr ;
    void* status ;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    cerr << "Main: Creating camera thread" << endl ;
    rc0 = pthread_create(&thread0,&attr,camera_thread,(void *)0);
    if (rc0){
        cerr << "Main: Error: Unable to create camera thread " << rc0 << endl ;
		if (errlog.good()) errlog << "Main: Error: Unable to create camera thread. errno" << errno << endl ;
        exit(-1) ; 
    }

	cerr << "Main: Creating housekeeping thread" << endl ;
    rc1 = pthread_create(&thread1,&attr,housekeeping_thread,(void *)1);
    if (rc1){
        cerr << "Main: Error: Unable to create housekeeping thread " << rc1 << endl ;
		if (errlog.good()) errlog << "Main: Error: Unable to create housekeeping thread. errno" << errno << endl ; 
    }

	#ifdef DATAVIS
	cerr << "Main: Creating datavis thread" << endl;
	rc2 = pthread_create(&thread2,&attr,datavis_thread,(void *)2);
    if (rc2){
        cerr << "Main: Error: Unable to create datavis thread " << rc2 << endl ;
		if (errlog.good()) errlog << "Main: Error: Unable to create datavis thread. errno" << errno << endl ;
    }
	#endif

    pthread_attr_destroy(&attr) ;

    rc0 = pthread_join(thread0,&status) ;
    if (rc0)
    {
        cerr << "Main: Error: Unable to join camera thread" << rc0 << endl ;
		if (errlog.good()) errlog << "Main: Error: Unable to join camera thread. errno" << errno << endl ;
        exit(-1);
    }
    cerr << "Main: Completed camera thread, exited with status " << status << endl ;

    rc1 = pthread_join(thread1,&status) ;
    if (rc1)
    {
        cerr << "Main: Error: Unable to join housekeeping thread" << rc1 << endl ;
		if (errlog.good()) errlog << "Main: Error: Unable to join housekeeping thread. errno" << errno << endl ;
        exit(-1);
    }
    cerr << "Main: Completed housekeeping thread, exited with status " << status << endl ;

	#ifdef DATAVIS
	rc2 = pthread_join(thread2,&status) ;
    if (rc2)
    {
        cerr << "Main: Error: Unable to join datavis thread" << rc2 << endl ;
		if (errlog.good()) errlog << "Main: Error: Unable to join datavis thread. errno" << errno << endl ;
        exit(-1);
    }
    cerr << "Main: Completed datavis thread, exited with status " << status << endl ;
	#endif
	#ifdef RPI
	gpioTerminate();
	#endif
    //pthread_exit(NULL);
    return 0 ;
}
