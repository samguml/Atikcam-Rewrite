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
