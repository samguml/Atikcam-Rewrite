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

