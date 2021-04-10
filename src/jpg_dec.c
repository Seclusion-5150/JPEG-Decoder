#include "jpg_dec.h"

void print_matrix(byte  **matrix, ulong len)
{
	int i = 0, j = 0;
	for(i = 0; i < len; i++)
	{
		for(j = 0; j < 2; j++)
		{
			printf("[%d][%d]: %#1x\n",i, j, (unsigned int)matrix[i][j]);
		}
		printf("\n");
	}
}
void print_array(byte *array, int len, char *type)
{
	int i;

	if(!strcmp(type, "Character")) // if strcmp is equal to 0 then it is true. Note: 0 in C is FALSE which is why I use the '!' symbol
	{
		for(i = 0; i < len;i++)
		{
			printf("%c",array[i]);
		}
	}
	else if(!strcmp(type, "Integer"))
	{
		for(i = 0; i < len; i++)
		{
			printf("%d", array[i]);
		}

	}
	else if (!strcmp(type, "Hexadecimal"))
	{
		for(i = 0; i < len; i++)
		{
			printf("%#1x ", array[i]);
		}
	}
	printf("\n");
}
void parse_tiff_header(struct file_data *data)
{
	if(exif.header.ifd0_ptr == 8)// if the header has already been filled set isFilled to true and return to the main function
	{
		exif.header.ifd0_ptr = TRUE;
		return;
	}
	byte c = 1;

	/*
	Exif(TIFF) Header File format
	Note: Some of the bytes contained in each section are zeros and in this implementation i skip over storing the zeros. For example instead of the Identifier being 6 bytes I have it store 5 bytes
	TODO: first  the Identifier [Not stored] 6 bytes
	second  the Endianness			 2 bytes
	third   the Signature			 2 bytes A fixed value
	fourth  the IFD0Pointer			 4 bytes A offset of the 0th IFD [Usually 8 not stored] // points to the beginning of the Header which starts at the address of the Endianness
	*/

	int i = 0, j = 0;

	while(c != 0xFF)
	{

		fscanf(data->image, "%c", &c);

		if(c != 0) // it seems that the Exif format uses 0s for padding between information so I will filter them out
		{
			if ( i < 5)
			{
				exif.header.identifier[i++] = c;
			}
			else if (( i >=5 ) && ( i < 7 ))
			{

				exif.header.endianness[j++] = c;
				i++;
			}
			else if (( i >= 7 ) && ( i < 8))
			{
				exif.header.signature = c;
				i++;
			}
			else if (i == 8)
			{
				exif.header.ifd0_ptr = c;
				i++;
			}
		}
	}

	printf("Identifier: ");
	print_array(exif.header.identifier, 5, "Character");
	printf("Endianness: ");
	print_array(exif.header.endianness, 2, "Character");
	printf("Signature: %d\n", exif.header.signature);
	printf("IFD0 Pointer: %d\n", exif.header.ifd0_ptr);

	fseek(data->image, -1L, SEEK_CUR);// Goes back one so that when it returns to the main function it will be able to parse the next marker
}

void parse_app1_subifd(struct file_data * data)
{

}
int main(int argc, char *argv[])
{
	//system("cd ..");
	exif.header.isFilled = FALSE;
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
						printf("\nEnd of Image: %#1x\n", (uint)c);
						data.isOver = TRUE;
						break;
					case APP0:
						printf("\nThis is a JFIF file: %#1x\n", (uint)c);
						break;
					case APP1:
						printf("\nThis is an EXIF metadata Structure: %#1x\n",(uint)c);
						if(!exif.header.isFilled) parse_tiff_header(&data);
						else parse_app1_subifd(&data);
						break;
					case APP12:
						printf("\nThis is the 0th IFD\n");
						break;
					case SOF0:
						printf("\nStart of frame(baseline DCT): %#1x\n", (uint)c);
						break;
					case SOF2:
						printf("\nStart of frame(progressive DCT): %#1x\n", (uint)c);
						break;
					case DHT:
						printf("\nThis specifies one or more Huffman tables: %#1x\n", (uint)c);
						break;
					case DQT:
						printf("\nThis specifies one or more quantization tables: %#1x\n", (uint)c);
						break;
					case DRI:
						printf("\nThis specifies the interval between RSTn markers in MCUs: %#1x\n", (uint)c);
						break;
					case SOS:
						printf("\nStart of Scan: %#1x\n", (uint)c);
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
