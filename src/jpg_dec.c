#include "jpg_dec.h"

void print_matrix(byte matrix[8][8])
{
	int i = 0, j = 0;
	printf("Quantization Table: \n");
	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			printf("%#1x ", (byte)matrix[i][j]);
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
		exif.header.isFilled = TRUE;
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
			else if (( i > 4 ) && ( i < 7 ))
			{

				exif.header.endianness[j++] = c;
				i++;
			}
			else if (i == 7)
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

void parse_xmp_segment(struct file_data * data)
{
	byte c = 1;
	int i = 0;
	bool isData = FALSE, isFirst = TRUE;
	char *string = NULL;
	char *temp = NULL;
	char **temp2 = NULL;
	int string_length = 0;
	exif.xmp.xpacket_length = 0;

	//Store the XMP identifier which is null terminated and always 29 bytes then print
	while(c != '\0')
	{
		fscanf(data->image, "%c", &c);
		exif.xmp.identifier[i++] = c;
	}
	printf("XMP Identifier: ");
	print_array(exif.xmp.identifier, 29, "Character");

	//Store the XMP packet
	while(c!=0xFF)
	{
		fscanf(data->image, "%c", &c);
		if(c == '<')
		{
			isData = TRUE;
			temp = create_array_char(c);
			if(temp == NULL)
			{
				printf("Error String is NULL\n");
			}
			else string = temp;
			string_length++;
		}
		else if(c == '>')
		{
			isData = FALSE;
			temp = add_item_char(string, c, string_length++);
			if(temp == NULL)
			{
				printf("Error String is NULL\n");
			}
			else string = temp;

			if(isFirst)
			{
				temp2 = create_array_string(string);
				if(temp == NULL)
				{
					printf("Error pointer is NULL\n");
				}
				else exif.xmp.xpacket = temp2;
				exif.xmp.xpacket_length++;
				isFirst = FALSE;
			}
			else
			{
				temp2 = add_item_string(exif.xmp.xpacket, string, exif.xmp.xpacket_length);
				if(temp2 == NULL)
				{
					printf("Error pointer is NULL\n");
				}
				else exif.xmp.xpacket = temp2;

				exif.xmp.xpacket_length++;
			}
			string_length = 0;
		}
		else if(isData)
		{
			temp = add_item_char(string, c, string_length++);
			if(temp == NULL)
			{
				printf("Error pointer is NULL\n");
			}
			else string = temp;
		}
	}
	//printf("\n");
	fseek(data->image, -1L, SEEK_CUR);
}
void parse_qtables(struct file_data * data)
{
	byte c = 1;
	int i = 0, j = 0;
	int counter = 0, counter2 = 0;
	int length = 0;
	fscanf(data->image, "%c", &c);


	bool isFirst = TRUE;
	while(c != 0xFF)
	{
		fscanf(data->image, "%c", &c);

			if(c != 0 && counter <= 1)
			{
				length+=c;
				counter++;
			}
			else if(counter > 1 && counter <= 2)
			{
				printf("Precision = %d bits\n", c << 4);
				counter++;
			}
			else
			{
				if(isFirst) pix_data.q_table1[i][j] = c;

				else if(!isFirst) pix_data.q_table2[i][j] = c;

				if(j > 7)
				{
					j = 0;
					i++;
				}
				if(i > 7)
				{
					if(isFirst == TRUE )i = 0;
					isFirst = FALSE;
				}
				j++;
			}
	}
	printf("Quantization Table Length: %d\n", length);
	fseek(data->image, -1L, SEEK_CUR);
}
void parse_dct(struct file_data *data)
{

}
int main(int argc, char *argv[])
{
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
						if(exif.header.isFilled)
						{
							parse_xmp_segment(&data);
							print_list_string(exif.xmp.xpacket, exif.xmp.xpacket_length);
						}
						break;
					case APP12:
						printf("\nThis is the 0th IFD\n");
						break;
					case SOF0:
						printf("\nStart of frame(baseline DCT): %#1x\n", (uint)c);
						parse_dct(&data);
						break;
					case SOF2:
						printf("\nStart of frame(progressive DCT): %#1x\n", (uint)c);
						break;
					case DHT:
						printf("\nThis specifies one or more Huffman tables: %#1x\n", (uint)c);
						break;
					case DQT:
						printf("\nThis specifies one or more quantization tables: %#1x\n", (uint)c);
						parse_qtables(&data);
						print_matrix(pix_data.q_table1);
						print_matrix(pix_data.q_table2);
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
