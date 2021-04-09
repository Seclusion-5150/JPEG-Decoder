#include "jpg_dec.h"

void print_matrix(byte  **matrix, ulong len)
{
	int i = 0, j = 0;
	for(i = 0; i < len; i++)
	{
		for(j = 0; j < 2; j++)
		{
			printf("[%d][%d]: %#1x\n",i, j, (unsigned int)matrix[i][j]);
			//getchar();
		}
		printf("\n");
	}
}
void print_array(byte *array, int len)
{
	int i;
	for(i = 0; i < len;i++)
	{
		printf("%c",array[i]);
	}
	printf("\n");
}
void parse_appn(struct file_data *data)
{
	byte c = 1;
	//byte string[4096];
	//fscanf(data->image, "%c", &c);
	
	/*
	Exif File format
	TODO: first  the Identifier [Not stored] 6 bytes
	second  the Endianness			 2 bytes
	third   the Signature			 2 bytes A fixed value
	fourth  the IFD0Pointer			 4 bytes A offset of the 0th IFD [Usually 8 not stored]
	fifth   IFD0				 ~ bytes Main image IFD
	sixth	IFD0@SubIFD			 ~ bytes Exif private tags [Optional, linked by IFD0]
	seventh IFD0@SubIFD@Interop		 ~ bytes Interoperability IFD [Optional, linked by SubIFD]
	eight	IFD0@GPS			 ~ bytes GPS IFD [Optional, linked by IFD0]
	ninth   APP1@IFD1			 ~ bytes Thumbnail IFD[Optional, pointed to by IFD0]
	tenth	ThumbnailData			 ~ bytes Thumbnail image [Optional, 0xffd8(SOI) ... 0xffd9(EOI)]
	*/
	/*
	Exif XMP format(Adobe Photoshop)
	TODO: I need to research this more
	*/
	int i = 0, j = 0, k = 0;
	while(c!=' ')
	{

		fscanf(data->image, "%c", &c);

		if(c!=0) // temporary if statements until I figure out how to properly parse the Exif header but it should work for now
		{

			if ( i < 5)
			{
				exif.identifier[i++] = c;
			}
			else if (( i >= 5 ) && ( i < 8 ))
			{

				exif.endianness[j++] = c;
				i++;
			}
		}
		if (( i >= 8 ) && ( i < 11))
		{
				exif.signature[k++] = c;
				i++;
		}

	}
	printf("Identifier: ");
	print_array(exif.identifier, 6);
	printf("Endianness: ");
	print_array(exif.endianness, 2);
	printf("Signature: ");
	print_array(exif.signature,  2);
}
int main(int argc, char *argv[])
{
	system("cd ..");
	struct file_data data;
	data.image = fopen("./res/mouse.jpg", "rb"); // put the path to your image here. 
	data.isOver = FALSE;
	fseek(data.image, 0, SEEK_END);
	data.file_len = ftell(data.image);
	rewind(data.image);

	byte c;
	int i = 0;
	while(!data.isOver)
	{
		fscanf(data.image,"%c", &c);
		if(c == 0xff)
		{
			fscanf(data.image, "%c", &c);// scan the image for markers located in the .h file
			switch (c)
			{
					case SOI:
						printf("Start of Image: %#1x\n", (uint)c);
						break;
					case EOI:
						printf("End of Image: %#1x\n", (uint)c);
						data.isOver = TRUE;
						break;
					case APP0:
						printf("This is a JFIF file: %#1x\n", (uint)c);
						parse_appn(&data);
						break;
					case APP1:
						printf("This is an EXIF metadata Structure: %#1x\n",(uint)c);
						parse_appn(&data);
						break;
					case SOF0:
						printf("Start of frame(baseline DCT): %#1x\n", (uint)c);
						break;
					case SOF2:
						printf("Start of frame(progressive DCT): %#1x\n", (uint)c);
						break;
					case DHT:
						printf("This specifies one or more Huffman tables: %#1x\n", (uint)c);
						break;
					case DQT:
						printf("This specifies one or more quantization tables: %#1x\n", (uint)c);
						break;
					case DRI:
						printf("This specifies the interval between RSTn markers in MCUs: %#1x\n", (uint)c);
						break;
					case SOS:
						printf("Start of Scan: %#1x\n", (uint)c);
						break;
					case RST0:
						break;

					case RST1:
						break;
					case RST2:
						break;
					case RST3:
						break;
					case RST4:
						break;
					case RST5:
						break;
					case RST6:
						break;
					case RST7:
						printf("Restart: %#1x\n", (uint)c);
						break;
					case COM:
						printf("This is a comment: %#1x\n", (uint)c);
						break;

			}
		}
		i++;
	}
	data.file_len = i;


	fclose(data.image);
	return 0;
}
