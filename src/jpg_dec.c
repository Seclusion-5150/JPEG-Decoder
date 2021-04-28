#include "arraylist.h"


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
	if(data->image_data.exif.header.ifd0_ptr == 8)// if the header has already been filled set isFilled to true and return to the main function
	{
		data->image_data.exif.header.isFilled = TRUE;
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

	while(c != 0xff)
	{

		c = fgetc(data->image);

		if(c != 0) // it seems that the Exif format uses 0s for padding between information so I will filter them out
		{
			if ( i < 5)
			{
				data->image_data.exif.header.identifier[i++] = c;
			}
			else if (( i > 4 ) && ( i < 7 ))
			{

				data->image_data.exif.header.endianness[j++] = c;
				i++;
			}
			else if (i == 7)
			{
				data->image_data.exif.header.signature = c;
				i++;
			}
			else if (i == 8)
			{
				data->image_data.exif.header.ifd0_ptr = c;
				i++;
			}
		}
	}

	printf("Identifier: ");
	print_array(data->image_data.exif.header.identifier, 5, "Character");
	printf("Endianness: ");
	print_array(data->image_data.exif.header.endianness, 2, "Character");
	printf("Signature: %d\n", data->image_data.exif.header.signature);
	printf("IFD0 Pointer: %d\n", data->image_data.exif.header.ifd0_ptr);

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
	data->image_data.exif.xmp.xpacket_length = 0;

	//Store the XMP identifier which is null terminated and always 29 bytes then print
	while(c != '\0')
	{
		c = fgetc(data->image);
		data->image_data.exif.xmp.identifier[i++] = c;
	}
	printf("XMP Identifier: ");
	print_array(data->image_data.exif.xmp.identifier, 29, "Character");

	//Store the XMP packet
	while(c!=0xff)
	{
		c = fgetc(data->image);
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
				else data->image_data.exif.xmp.xpacket = temp2;
				data->image_data.exif.xmp.xpacket_length++;
				isFirst = FALSE;
			}
			else
			{
				temp2 = add_item_string(data->image_data.exif.xmp.xpacket, string, data->image_data.exif.xmp.xpacket_length);
				if(temp2 == NULL)
				{
					printf("Error pointer is NULL\n");
				}
				else data->image_data.exif.xmp.xpacket = temp2;

				data->image_data.exif.xmp.xpacket_length++;
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
	fseek(data->image, -1L, SEEK_CUR);
}
void parse_qtables(struct file_data * data)
{
	byte table_info = 0;
	int i = 0, j = 0;
	byte length = 0;
	byte data2[8][8];
	byte array_length = 0;
	length  = fgetc(data->image) << 8;
	length += fgetc(data->image);
	length-=2;
	printf("Quantization Table Length: %d\n", length);
	while(length > 0)
	{
		table_info = fgetc(data->image);
		byte table_id = table_info & 0x0f;
		if(table_id > 3)
		{
			printf("Error: %#1x\n", table_id);
			return;
		}
		if(table_info >> 4 !=0)
		{
			for(i = 0; i < 64;i++)
			{
					data->image_data.q_tables[array_length].q_table[zz_map[i]] = (fgetc(data->image) << 8) + fgetc(data->image);
			}
			data->image_data.q_tables[array_length].isFilled = TRUE;

			array_length++;

			length-=128;
		}
		else
		{
			for(i = 0; i < 64;i++)
			{
					data->image_data.q_tables[array_length].q_table[zz_map[i]] = fgetc(data->image);
			}
			data->image_data.q_tables[array_length].isFilled = TRUE;

			array_length++;
			length -=64;
		}

		length--;
	}
}
void parse_dct(struct file_data *data)
{

	byte c = 1;
	int i = 0, j = 0;
	byte component_id;

	//get length
	uint length = fgetc(data->image) << 8;
	length+= fgetc(data->image);

	//get precision
	byte precision = fgetc(data->image); // must be 8 jpeg doesn't support anything other than 8 bit precision


	data->image_data.width = 0;
	data->image_data.height = 0;
	data->image_data.color_type = 0;
	//get length
	printf("Length: %u\n", length);

	//get width
	data->image_data.width += fgetc(data->image) << 8;							//(Width or Height) = (b1 * 256(2^8)) + b2
	data->image_data.width += fgetc(data->image); // b1 being the first byte																	//and b2 being the second byte

	//get height
	data->image_data.height += fgetc(data->image) << 8;
	data->image_data.height += fgetc(data->image);

	//get color type CMYK, YCbCr, or Y
	data->image_data.color_type = fgetc(data->image);

	for(j = 0; j < data->image_data.color_type;j++)
	{
			component_id = fgetc(data->image);
			c = fgetc(data->image);
			data->image_data.ccmp[component_id - 1].horizontal_sampling_factor = c >> 4; //Get the upper bits by shifting to the right
			data->image_data.ccmp[component_id - 1].vertical_sampling_factor = c & 0x0F;// Get the lower bits by &ing with 0x0F
			data->image_data.ccmp[component_id - 1].quantization_table_id = fgetc(data->image);
	}
	length = length - 8 - (3*data->image_data.color_type);
	if(length != 0)
	{
		printf("Error\n");
		return;
	}
	printf("Width: %u\n", data->image_data.width);
	printf("Height: %u\n", data->image_data.height);
	fseek(data->image, -1L, SEEK_CUR);
}

void parse_htables(struct file_data *data)
{
	byte c = 1;
	int counter = 0;
	int length = fgetc(data->image) << 8;
	length+= fgetc(data->image);

	printf("length: %d\n", length);
	length-=2;
	while(length > 0)
	{
		c = fgetc(data->image);



		length--;
	}
}

int main(int argc, char *argv[])
{
	//get the file to read from the commandline
	char *args = malloc(sizeof(char)*(strlen("./res/") + strlen(argv[1])));
	strcat(args, "./res/");
	strcat(args, argv[1]);

	struct file_data data;
	int i = 0, j = 0;
	for(i = 0; i < 4;i++) data.image_data.q_tables[i].isFilled = FALSE;

	data.image_data.exif.header.isFilled = FALSE;

	data.image = fopen(args, "rb"); // put the path to your image here.
	data.isOver = FALSE;
	fseek(data.image, 0, SEEK_END);
	data.file_len = ftell(data.image);
	rewind(data.image);

	byte c;
	int counter = 0;
	while(!data.isOver)
	{
		fscanf(data.image,"%c", &c);
		if(c == 0xff)
		{
			c = fgetc(data.image);// scan the image for markers located in the .h file
			switch (c)
			{
					case SOI:
						printf("Start of Image: %#1x\n", (uint)c);
						break;
					case EOI:
						printf("\nEnd of Image: %#1x\n", (uint)c);
						data.isOver = TRUE;
						counter++;
						break;
					case APP0:
						printf("\nThis is a JFIF file: %#1x\n", (uint)c);
						break;
					case APP1:
						printf("\nThis is an EXIF metadata Structure: %#1x\n",(uint)c);
						if(!data.image_data.exif.header.isFilled) parse_tiff_header(&data);
						if(data.image_data.exif.header.isFilled)
						{
							parse_xmp_segment(&data);
							print_list_string(data.image_data.exif.xmp.xpacket, data.image_data.exif.xmp.xpacket_length);
						}
						break;
					case APP12:
						printf("\nThis is the 0th IFD\n");
						break;
					case SOF0:
						printf("\nStart of frame(baseline DCT): %#1x\n", (uint)c);
						parse_dct(&data);
						i = 0, j = 0;
						for(i = 0; i < 3;i++)
						{
							printf("Component ID: %d\n", i + 1);
							printf("Horizontal Sampling Factor: %d\n", data.image_data.ccmp[i].horizontal_sampling_factor);
							printf("Vertical Sampling Factor: %d\n", data.image_data.ccmp[i].vertical_sampling_factor);
							printf("Quantization Table ID: %d\n", data.image_data.ccmp[i].quantization_table_id);
							printf("\n");
						}
						switch(data.image_data.color_type)
						{
							case 4:
								printf("Color Type: CMYK\n");
								break;
							case 3:
								printf("Color Type: YCbCr");
								break;
							case 1:
								printf("Color Type: Y\n");
								break;
						}
						break;
					case SOF2:
						printf("\nStart of frame(progressive DCT): %#1x\n", (uint)c);
						break;
					case DHT:
						printf("\nThis specifies one or more Huffman tables: %#1x\n", (uint)c);
						parse_htables(&data);
						break;
					case DQT:
						printf("\nThis specifies one or more quantization tables: %#1x\n", (uint)c);
						parse_qtables(&data);
						j = 0;
						i = 0;
						int counter = 0;
						while(data.image_data.q_tables[i].isFilled)
						{
								printf("Quantization Table: \n");
								for(j = 0; j < 64;j++)
								{
									printf("%#1x ", data.image_data.q_tables[i].q_table[j]);
									counter++;

									if(counter == 8)
									{
										counter = 0;
										printf("\n");
									}
								}
								printf("\n");
								i++;
						}
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
