/**
@file main.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief provides threat init, spawn, join, destroy. Signal handler for all threads. Would contain error printer but that feature has been phased out 
@Date 9/28/2020

**/
#define MAIN_PRIVATE
#include <main.h>
#include <modules.h>
#undefine MAIN_PRIVATE
#include <cstdlib>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <pigpio.h>
# include <fstream>
#include <sys/types.h>
#include <ofstream>
#include <boost/filesystem.hpp>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <atikccdusb.h>
#include <limits.h>

bool gpio_status;
char curr_dir[PATH_MAX];
volatile sig_atomic_t done = 0;

//Memory allocation for logs
ofstream templog ;
ofstream camlog ;
ofstream errlog ;


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
	
  /* Set up interrupt handler Note: This handler comes from shflight and replaced the one that was in use. */

    struct sigaction saction;
    saction.sa_handler = &catch_sigint;
    sigaction(SIGINT, &saction, NULL);
	
// SIGINT handler-replaced old signal handler. This signal handler is inactive, as there are no conditional wakeups in the flightcam code. 
void catch_sigint(int sig)
{
    done = 1;
    for (int i = 0; i < num_wakeups; i++)
        pthread_cond_broadcast(wakeups[i]);
}
	
 //End set up interrupt handler 

	
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


#ifdef ENABLE_PWOFF
void sys_poweroff(void)
{
	cerr << "Info: Poweroff instruction received!" << endl ;
	sync() ;
	setuid(0) ;
	reboot(LINUX_REBOOT_CMD_POWER_OFF) ;
}
#else
void sys_poweroff(void)
{	
	cerr << "Info: Poweroff instruction received!" << endl ;
	exit(0);
}
#endif //ENABLE_PWOFF
