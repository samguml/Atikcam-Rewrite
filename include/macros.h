/*
@file macros.h
@author: 9/28/20
@brief: contains all macros
*/ 

#include <stdio.h>

#define MAX 1

#ifndef MAX_ALLOWED_EXPOSURE
#define MAX_ALLOWED_EXPOSURE 40
#endif

#ifndef TIME_WAIT_USB
#define TIME_WAIT_USB 1000000
#endif

#ifndef PIC_TIME_GAP
#define PIC_TIME_GAP 10.0 
#endif

#ifndef PIX_MEDIAN
#define PIX_MEDIAN 45000.0
#endif

#ifndef PIX_GIVE
#define PIX_GIVE 5000.0
#endif

#ifndef PIX_BIN
#define PIX_BIN 1
#endif

#ifndef CCD_COOLDOWN_TIME
#define CCD_COOLDOWN_TIME 60*1000 
#endif

#ifndef PORT
#define PORT 12376
#endif //PORT

#ifndef SERVER_IP
#define SERVER_IP "192.168.1.2"
#endif //SERVER_IP

#ifndef PACK_SIZE
#define PACK_SIZE sizeof(datavis_p)
#endif

#ifndef PERCENTILE
#define PERCENTILE 90.0
#endif

#ifndef PIX_MEDIAN
#define PIX_MEDIAN 40000.0
#endif
  
#ifndef PIX_GIVE
#define PIX_GIVE 5000.0
#endif

#ifndef TEMPLOG_LOCATION
#define TEMPLOG_LOCATION "/home/pi/temp_log.bin"
#endif

#ifndef CAMLOG_LOCATION
#define CAMLOG_LOCATION "/home/pi/cam_log.bin"
#endif

#ifndef CHASSISLOG_LOCATION
#define CHASSISLOG_LOCATION "/home/pi/chassis_log.bin"
#endif


#ifndef BOARDLOG_LOCATION
#define BOARDLOG_LOCATION "/home/pi/board_log.bin"
#endif


#ifndef BOARDLOG_LOCATION
#define BOARDLOG_LOCATION "/home/pi/board_log.bin"
#endif
  
  
 
  


