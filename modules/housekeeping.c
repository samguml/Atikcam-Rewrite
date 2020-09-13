/**
@file housekeeping.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief 
@Date 9/13/2020
@copyright Copyright (c) 2020
**/

//Function prototypes are as follows

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

unsigned long long int timenow()
  {
	return ((std::chrono::duration_cast<std::chrono::milliseconds> 
	((std::chrono::time_point_cast<std::chrono::milliseconds>
	(std::chrono::system_clock::now())).time_since_epoch())).count()) ;
}

//again need to ask about this one
void * housekeeping_thread(void *t)
