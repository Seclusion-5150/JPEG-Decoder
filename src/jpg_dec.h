#include <stdlib.h>
#include <stdio.h>
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
typedef unsigned char byte;
typedef unsigned int  uint;
typedef unsigned long ulong;
struct pixel
{
	/*RGB can be computed directly from these values the problem is knowing how ...*/
	byte   y; // luminance
	byte  cb; // blue chrominance: How much blue is in the pixel
	byte  cr; // red chrominance: How much red is in the pixel
};
struct tiff_header
{
	byte identifier[5];
	byte endianness[2];
	byte signature;
	byte ifd0_ptr;
	bool isFilled;
};

struct xmp_segment
{
	byte identifier[29];
	char **xpacket;
	int xpacket_length;
};
struct exif_data
{
	struct tiff_header header;
	struct xmp_segment xmp;
};
struct MCUs
{
	struct pixel mcu[8][8];
};
struct quantization_table
{
	byte q_table[64];
	bool isFilled;
};
const byte zz_map[] =
{
	0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12,
	19, 26, 33, 40,48, 41, 34, 27, 20, 13,6, 7, 14, 21, 28, 35,
	42, 49,56, 57, 50, 43, 36,29, 22, 15, 23, 30, 37, 44, 51, 58,
	59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};
struct color_component
{
	byte horizontal_sampling_factor;
	byte vertical_sampling_factor;
	byte quantization_table_id;
	byte ac_huffman_table_id;
	byte dc_huffman_table_id;
};
struct huffman_table
{
	byte **symbols;
	int rows;
	int symbol_sizes[16];
	bool isFilled;
	struct htree_codes *codes;
};
struct htree_codes
{
	struct htree_codes *left_child;
	struct htree_codes *right_child;
	bool isLeaf;
	bool isFilled;
	int code;
	uint code_length;
};

struct image_data
{
	int width;
	int height;
	byte color_type;
	struct color_component ccmp[4];
	struct quantization_table q_tables[4];
	struct exif_data exif;
	struct huffman_table ac_tables[4];
	struct huffman_table dc_tables[4];
	byte *huffman_data;
	int huffman_data_len;
	byte start_of_sel;
	byte end_of_sel;
	byte successive_approximation;
	byte sa_high;
	byte sa_low;
};

struct file_data
{
	FILE *image;
	unsigned long file_len;
	bool isOver;
	struct image_data image_data;
};
