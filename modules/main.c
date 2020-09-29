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