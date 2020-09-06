/**
@file main.h
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief 
@Date 9/3/2020

@copyright Copyright (c) 2020

**/


//@brief Internal Data Structure

typedef struct image {
	uint64_t tnow ; // timestamp in ms (from epoch, in local time)
	float exposure ; //exposure length in seconds
	unsigned short pixx ; //pixel x
	unsigned short pixy ; //pixel y
    unsigned int imgsize ; //pixx*pixy
    short ccdtemp ; // temp in C = temp/100
    short boardtemp ;
    short chassistemp ;
	unsigned short picdata[1449072] ;
	unsigned char unused[6] ; //first set of padding, default for GCC -> multiple of 32
	unsigned char unused2[1792] ; //padding to round off to 708*4096 bytes
} image ; //size 708*4096

//@brief Packet Serializer

typedef struct {
	uint64_t tnow ; //timestamp in ms
	float exposure ; //exposure in ms
	unsigned short pixx ; //348
	unsigned short pixy ; //260
	unsigned char pixbin ;
	short ccdtemp ; // temp in C * 100
	short boardtemp ;
	short chassistemp ;
	unsigned char picdata[90480];
} datavis_p ;
#ifndef PACK_SIZE
#define PACK_SIZE sizeof(datavis_p)
#endif
typedef union{
	datavis_p a ;
	unsigned char buf [sizeof(datavis_p) / PACK_SIZE] [PACK_SIZE];
} packetize ;
packetize global_p ;

//@brief Packet Converter 
void convert_to_packet(image * a , datavis_p * b)
{
	b -> tnow = a -> tnow ;
	b -> exposure = (a -> exposure) ;
	b -> pixx = 348 ;
	b -> pixy = 260 ;
	b -> pixbin = pix_bin ;
	b -> ccdtemp = a -> ccdtemp ;
	b -> boardtemp = a -> boardtemp ;
	b -> chassistemp = a -> chassistemp ;
	uint8_t numbin = (a->pixx) / (b->pixx) ;
	if (numbin == 1) //no downsample, just copy
		for (int i = 0 ; i < a -> imgsize ; i++ )
			b->picdata[i] = (unsigned char)((a->picdata[i]) / 256);
	else {
		for (unsigned short i = 0 ; i < 260 ; i++ )
		{
			for (unsigned short j = 0 ; j < 348 ; j++)
			{
				double temp = 0 ;
				for (unsigned char k = 0 ; k < numbin; k++)
					for ( unsigned char l = 0 ; l < numbin ; l++ )
						temp += a->picdata[i*numbin*348*numbin + j*numbin /*root of pixel*/
						+ 348*numbin*k /*y axis*/
						+l /* x axis */]*255.0/65535; //converted to 8 bit
				temp /= numbin * numbin ;
				b->picdata[i*348+j] = (unsigned char)temp;
			}
		}
	}
	return ;
}
//@brief
// Error handler prototype
void sherror(const char *msg)
//@brief

//@brief

