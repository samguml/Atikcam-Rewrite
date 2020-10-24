/**
@file housekeeping.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief 
@Date 9/13/2020
@copyright Copyright (c) 2020
**/

#include <housekeeping.h>
#include <main.h>
#include <camera_extern.h>
#include <chrono>
#include <cmath>
#include <pigpio.h>


 //Allocation of memory for cross-module variables  
volatile int boardtemp;

volatile int chassistemp ;

volatile unsigned long long camofftime = INFINITY ;

bool cam_off = false;

volatile bool ccdoverheat = false;

//Routine declarations

unsigned long long int timenow()
{
	return ((std::chrono::duration_cast<std::chrono::milliseconds> 
	((std::chrono::time_point_cast<std::chrono::milliseconds>
	(std::chrono::system_clock::now())).time_since_epoch())).count()) ;
}
inline void put_data ( ostream & str , unsigned short val )
{
	shb x ;
	x.s = val ;
	for ( char i = 0 ; i < sizeof(x.b) ; i++ )
		str << x.b[i] ;
}

inline void put_data ( ostream & str , unsigned long long int val )
{
	llb x ;
	x.l = val ;
	for ( char i = 0 ; i < sizeof(x.b) ; i++ )
		str << x.b[i] ;
}

inline void put_data ( ostream & str , float val )
{
	flb x ;
	x.f = val ;
	for ( char i = 0 ; i < sizeof(x.b) ; i++ )
		str << x.b[i] ;
}
 inline void put_data ( ostream & str , char val )
 {
	 str << val ;
 }

void * housekeeping_thread(void *t)
{   
	ofstream tempchassis, tempboard ;
	#ifndef CHASSISLOG_LOCATION
	#define CHASSISLOG_LOCATION "/home/pi/chassis_log.bin"
	#endif

	tempchassis.open( CHASSISLOG_LOCATION , ios::binary | ios::app ) ;
	bool chassislogstat = true ;
	if ( !tempchassis.good() )
	{
		cerr << "Housekeeping: Unable to open chassis temperature log stream." << endl ;
		chassislogstat = false ;
	}
	cerr << "Housekeeping: Opened chassis temperature log file" << endl ;
	tempchassis << "Data format: " << endl
				<< "tnow (ulonglong, 8 bytes) followed by temperature (in C, no decimal, 1 byte)" << endl ; 
	
	#ifndef BOARDLOG_LOCATION
	#define BOARDLOG_LOCATION "/home/pi/board_log.bin"
	#endif

	tempboard.open( BOARDLOG_LOCATION , ios::binary | ios::app ) ;
	bool boardlogstat = true ;
	if ( !tempboard.good() )
	{
		cerr << "Housekeeping: Unable to open board temperature log stream." << endl ;
		boardlogstat = false ;
	}
	cerr << "Housekeeping: Opened board temperature log file" << endl ;
	tempchassis << "Data format: " << endl
				<< "tnow (ulonglong, 8 bytes) followed by temperature (in C, no decimal, 1 byte)" << endl ;
	/*
	 * List of addresses:
	 * 1. ADS1115: 0x48
	 * 2. MCP9808: 0x18 (open), 0x19 (A0), 0x1a (A1), 0x1c (A2)
	 * 3. 10 DOF Sensor: 0x19, 0x1E, 0x6B, 0x77
	*/
	mcp9808 therm[2];
	ads1115 csensor = ads1115(0x48/* Default: 0x48 or hex('H')*/) ;
	bool masteractive = therm[0].begin(0x18) ; therm[1].begin(0x1a);
	csensor.begin();
	csensor.setGain(GAIN_ONE);
	while(!done){
		unsigned long long tnow = timenow() ;
		boardtemp = therm[0].readTemp();
		chassistemp = therm[1].readTemp() ;
		if ( masteractive ){
		/* Board too hot or too cold: Turn off camera */
		if ((boardtemp/100. > 50||boardtemp/100. < 0) && cam_off == false)
		{
			#ifdef RPI
			gpioWrite(17,0);
			#endif
			if (camlog.good()) camlog << tnow << " board overheat detected, turning off camera" << endl ;
			cam_off = true ;
		}
		/**********************************************/

		/* Board has cooled down too far, turn on heater */
		while ( boardtemp/100. < 0 && cam_off == true ) //board too cold and camera is off
		{
			#ifdef RPI
			gpioWrite(27,1); //turn on heater
			#endif
			usleep(5000000) ; //500 ms
			#ifdef RPI
			gpioWrite(27,0) ; //turn off heater
			#endif
			boardtemp = therm[0].readTemp();
		}
		/*************************************************/

		/* Camera off because of CCD over heat and cooldown period has passed */
		if ( cam_off && ccdoverheat && (camofftime - timenow()<CCD_COOLDOWN_TIME))
		{
			#ifdef RPI
			gpioWrite(17,1);
			#endif
			if (camlog.good()) camlog << tnow << " CCD Cooldown period passed, turning on camera" << endl ;
			cam_off = false ;
			ccdoverheat = false ;
			camofftime = INFINITY ;
		}
		/**********************************************************************/

		/* Board temperature okay and camera is off, turn on camera */
		if ( (boardtemp/100. < 50 && boardtemp/100. > 0) && cam_off == false )
		{
			cam_off = false ;
			#ifdef RPI
			gpioWrite(17,1);
			#endif
		}
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		} //endif masteractive
		else{
			/* Camera off because of CCD over heat and cooldown period has passed */
			if ( cam_off && ccdoverheat && (camofftime - timenow()<CCD_COOLDOWN_TIME))
			{
				#ifdef RPI
				gpioWrite(17,1);
				#endif
				if (camlog.good()) camlog << tnow << " CCD Cooldown period passed, turning on camera" << endl ;
				cam_off = false ;
				ccdoverheat = false ;
				camofftime = INFINITY ;
			}
			/**********************************************************************/
			else {
				#ifdef RPI
				gpioWrite(17,1);
				cam_off = false ;
				#endif
			}
		}
		// cerr << "Housekeeping: Board Temp: " << boardtemp / 100.0 << " C,"
		// 	 << " Chassis Temp: " << chassistemp / 100.0 << " C" << endl ; 
		if (boardlogstat){
			put_data(tempboard,tnow);
			put_data(tempboard,(char)(boardtemp/100));
		}
		if (chassislogstat){
			put_data(tempchassis,tnow);
			put_data(tempchassis,(char)(chassistemp/100));
		}
		int16_t adc0, adc1 ;
		adc0 = csensor.readADC_SingleEnded(0);
		adc1 = csensor.readADC_SingleEnded(1);
		// cerr << "Housekeeping: ADC:" << endl
		// 	 << "Housekeeping: A0: " << adc0 << " V" << endl
		// 	 << "Housekeeping: A1: " << adc1 << " V" << endl ; //uncalibrated voltage for current
		usleep(1000000); //every 1s
	}
	#ifdef RPI //ensure camera and heater are OFF
	gpioWrite(17,0) ;
	gpioWrite(27,0) ;
	#endif
    pthread_exit(NULL) ;
}
