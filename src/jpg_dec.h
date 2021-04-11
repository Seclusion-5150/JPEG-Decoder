#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0


// Format 0xFFXX [YYYY ...]

#define SOI  	0xd8 // start of image
#define EOI  	0xd9 // end of image

#define SOF0	0xC0 //variable size
#define SOF2	0xC2 //start of frame: indicates that this is a baseline DCT-Based JPEG, and specifies the width, height, number of components

#define DHT  	0xC4 //specifies one or more huffman tables
#define DQT  	0xDB //specifies one or more quantization tables
#define DRI  	0xDD //specifies the interval between RSTn markers, in Minimum coded Units(MCUs)

#define SOS  	0xDA // start of scan
//Restart markers the low bits values being from 0 to 7
#define RST0 	0xD0
#define RST1 	0xD1
#define RST2 	0xD2
#define RST3 	0xD3
#define RST4 	0xD4
#define RST5 	0xD5
#define RST6 	0xD6
#define RST7 	0xD7
//Application Specific markers
#define APP0 	0xE0 //JFIF marker
#define APP1 	0xE1 //EXIF marker
#define APP2 	0xE2
#define APP3 	0xE3
#define APP4 	0xE4
#define APP5 	0xE6
#define APP6 	0xE7
#define APP7 	0xE8
#define APP8 	0xE9
#define APP9 	0xE10
#define APP10	0xE11
#define APP11 	0xE12
#define APP12 	0xEC
#define APP13 	0xE14

#define COM  	0xFE //contains a comment

typedef int bool;
typedef unsigned  char byte;
typedef unsigned  int  uint;
typedef unsigned  long ulong;

struct pixel_data
{
	/*RGB can be computed directly from these values the problem is knowing how ...*/
	int   y[8][8]; // luminance
	int  cb[8][8]; // blue chrominance: How much blue is in the pixel
	int  cr[8][8]; // red chrominance: How much red is in the pixel
};
struct image_data
{
	int width;
	int height;
	int isColor;
	struct pixel_Data** image;
};

struct file_data
{
	FILE *image;
	unsigned long file_len;
	bool isOver;
};
struct tiff_header
{
	byte identifier[5];
	byte endianness[2];
	byte signature;
	byte ifd0_ptr;
	bool isFilled;
};
struct xmp_header
{
	byte begin[40];
	byte id[40];
};
struct xmp_segment
{
	byte identifier[29];
	//add variable(s) for xmp packet here
	struct xmp_header header;
};
struct exif_data
{
	struct tiff_header header;
	struct xmp_segment xmp;
};

struct exif_data exif;
