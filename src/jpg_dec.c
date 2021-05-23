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
			printf("%#1x \n", array[i]);
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
	byte qtable_info = 0;
	int i = 0;
	byte length = 0;
	byte array_length = 0;
	length  = fgetc(data->image) << 8;
	length += fgetc(data->image);
	length -=2;
	printf("Quantization Table Length: %d\n", length);
	while(length > 0)
	{
		qtable_info = fgetc(data->image);
		byte qtable_id = qtable_info & 0x0f;
		if(qtable_id > 3)
		{
			printf("Error: %#1x\n", qtable_id);
			return;
		}
		if(qtable_info >> 4 != 0)
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
}

void parse_htables(struct file_data *data)
{
	byte c = 0;
	int i = 0, k = 0, p = 0;
	int length = fgetc(data->image) << 8;
	length += fgetc(data->image);
	printf("length: %d\n", length);
	length -= 2;

	while(length > 0)
	{

		byte htable_info = fgetc(data->image);
		byte htable_id   = htable_info & 0x0F;
		bool isACTable  = htable_info >> 4;

		length--;
		if(htable_id > 3)
		{
			printf("Error(%d): Invalid Huffman table?\n", htable_id);
			continue;
		}
		else
		{
			char *string = malloc(sizeof(char) * 20);
			if(isACTable)string = "an AC table";
			else string = "a DC table";
			printf("Huffman Table Codes for %s: %d\n",string, htable_id);
		}
		if(isACTable)
		{

			for(i = 0; i < 16;i++)
			{
				data->image_data.ac_tables[p].symbol_sizes[i] = fgetc(data->image);
				c += data->image_data.ac_tables[p].symbol_sizes[i];
			}

			length -= (16 + c);

			byte code = 0;
			int j = 0, l = 0, m = 0;
			data->image_data.ac_tables[p].isFilled = TRUE;

			data->image_data.ac_tables[p].symbols = (byte **)malloc(sizeof(byte*)*16);

			data->image_data.ac_tables[p].rows = 16;

			for(i = 0; i < 16; i++) data->image_data.ac_tables[p].symbols[i] = (byte *)malloc(sizeof(byte*)*data->image_data.ac_tables[p].symbol_sizes[i]);

			j = 0;
			while(c > 0)
			{
				for(i = 0; i < data->image_data.ac_tables[p].symbol_sizes[j];++i)
				{
					code = fgetc(data->image);
					data->image_data.ac_tables[p].symbols[l][m++] = code;
				}
				c-=data->image_data.ac_tables[p].symbol_sizes[j++];
				m = 0;
				l++;
			}
			p++;
			c = 0;
		}
		else
		{

			for(i = 0; i < 16;i++)
			{
				data->image_data.dc_tables[k].symbol_sizes[i] = fgetc(data->image);
				c += data->image_data.dc_tables[k].symbol_sizes[i];
			}

			length -= (16 + c);

			byte code = 0;
			int j = 0, l = 0, m = 0;
			data->image_data.dc_tables[k].isFilled = TRUE;
			data->image_data.dc_tables[k].symbols = (byte **)malloc(sizeof(byte*)*16);
			data->image_data.dc_tables[k].rows = 16;
			for(i = 0; i < 16; i++) data->image_data.dc_tables[k].symbols[i] = (byte *)malloc(sizeof(byte*)*data->image_data.dc_tables[k].symbol_sizes[i]);

			j = 0;
			while(c > 0)
			{
				for(i = 0; i < data->image_data.dc_tables[k].symbol_sizes[j];++i)
				{
					code = fgetc(data->image);
					data->image_data.dc_tables[k].symbols[l][m++] = code;
				}
				m = 0;
				l++;
				c-=data->image_data.dc_tables[k].symbol_sizes[j++];
			}
			c = 0;
			k++;
		}
	}
}
void scan_image(struct file_data * data)
{
	int length = fgetc(data->image) << 8;
	int num_components = 0;
	int i = 0;
	byte stream = 0, stream_last = 0;
	length += fgetc(data->image);
	printf("Length: %d\n", length);

	num_components = fgetc(data->image);
	printf("Number of Components: %d\n", num_components);
	for(i = 0; i < num_components; i++)
	{
		byte component_id = fgetc(data->image);
		byte htable_id = fgetc(data->image);

		data->image_data.ccmp[component_id - 1].ac_huffman_table_id = htable_id >> 4;
		data->image_data.ccmp[component_id - 1].dc_huffman_table_id = htable_id & 0x0F;

		if(data->image_data.ccmp[component_id - 1].ac_huffman_table_id > 3
		 || data->image_data.ccmp[component_id - 1].dc_huffman_table_id > 3)
		{
			printf("Error\n");
			return;
		}

	}
	data->image_data.start_of_sel = fgetc(data->image);
	data->image_data.end_of_sel = fgetc(data->image);
	data->image_data.successive_approximation = fgetc(data->image);
	data->image_data.sa_high = data->image_data.successive_approximation >> 4;
	data->image_data.sa_low = data->image_data.successive_approximation & 0x0F;

	if(data->image_data.start_of_sel != 0 || data->image_data.end_of_sel != 63)
	{
		printf("ERROR WITH START OF SELECTION OR END OF SELECTION\n");
		return;
	}
	if(data->image_data.sa_low != 0 || data->image_data.sa_high != 0)
	{
		printf("ERROR WITH SUCCESSIVE APPROXIMATION\n");
		return;
	}
	long counter = 0;
	bool isOver = FALSE;
	while(!isOver) // read all of the bytes after the start of scan marker until the end of image markers
	{							 //Count the length of the data
		stream = fgetc(data->image);
		++counter;
		if(stream == 0xFF)
		{
			stream = fgetc(data->image);
			++counter;
			switch(stream)
			{
				case EOI:
					isOver = TRUE;
					break;
			}
		}
	}

	fseek(data->image, -counter, SEEK_CUR); // go back to the beginning of the SOS marker
	data->image_data.huffman_data = (byte *)malloc(sizeof(byte) * (counter - 2));
	data->image_data.huffman_data_len = counter;
	stream = 0;
	for(i = 0; i < (counter - 2); i++)//Now read all data until right before the end of image marker
	{
		stream = fgetc(data->image);
		data->image_data.huffman_data[i] = stream;
		//Currently does not handle any reset markers
		//Will fix later
	}
}

void decomp_hdata(struct file_data *data)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	bool isRight = FALSE;

	while(data->image_data.dc_tables[k].isFilled)
	{
		data->image_data.dc_tables[k].codes = (struct htree_codes*)malloc(sizeof(struct htree_codes) * 1000);
		for(i = 0; i < 1000; i++)
		{
			data->image_data.dc_tables[k].codes[i].right_child = (struct htree_codes*)malloc(sizeof(struct htree_codes) * 1);
			data->image_data.dc_tables[k].codes[i].right_child->code = -1;
			data->image_data.dc_tables[k].codes[i].code_length = -1;
			data->image_data.dc_tables[k].codes[i].left_child = (struct htree_codes*)malloc(sizeof(struct htree_codes) * 1);
			data->image_data.dc_tables[k].codes[i].left_child->code = -1;
		}

		for(i = 0; i < 16; i++)
		{
			for(j = 0; j < data->image_data.dc_tables[k].symbol_sizes[i];j++)
			{
				if((j + 1) % 2 == 0)
				{
					data->image_data.dc_tables[k].codes[l].right_child->code = data->image_data.dc_tables[k].symbols[i][j];
					printf("Right: [%d][%d]: %d\n", i + 1, j, data->image_data.dc_tables[k].codes[l].right_child->code);
					++l;
					isRight = TRUE;
				}
				else
				{
					isRight = FALSE;
					data->image_data.dc_tables[k].codes[l].isFilled = TRUE;

					data->image_data.dc_tables[k].codes[l].code_length = i + 1;
					printf("Code Length: %d\n", data->image_data.dc_tables[k].codes[l].code_length);
					data->image_data.dc_tables[k].codes[l].left_child->code = data->image_data.dc_tables[k].symbols[i][j];
					printf("Left:  [%d][%d]: %d\n", i + 1, j, data->image_data.dc_tables[k].codes[l].left_child->code);

				}
			}
			if(data->image_data.dc_tables[k].symbol_sizes[i] != 0 && !isRight)l++;
		}
		for(i = 0; i < l; i++)
		{
			printf("    %d\n\n", data->image_data.dc_tables[k].codes[i].code_length);
			printf("%d	", data->image_data.dc_tables[k].codes[i].left_child->code );
			printf("%d\n\n", 	 data->image_data.dc_tables[k].codes[i].right_child->code);
		}
		l = 0;
		k++;
	}
	isRight = FALSE;
	k = 0;
	l = 0;
	while(data->image_data.ac_tables[k].isFilled)
	{
		data->image_data.ac_tables[k].codes = (struct htree_codes*)malloc(sizeof(struct htree_codes) * 1000);
		for(i = 0; i < 1000; i++)
		{
			data->image_data.ac_tables[k].codes[i].right_child = (struct htree_codes*)malloc(sizeof(struct htree_codes) * 1);
			data->image_data.ac_tables[k].codes[i].right_child->code = -1;
			data->image_data.ac_tables[k].codes[i].code_length = -1;
			data->image_data.ac_tables[k].codes[i].left_child = (struct htree_codes*)malloc(sizeof(struct htree_codes) * 1);
			data->image_data.ac_tables[k].codes[i].left_child->code = -1;
			data->image_data.ac_tables[k].codes[i].isLeaf = FALSE;
			data->image_data.ac_tables[k].codes[i].isFilled = FALSE;
		}

		for(i = 0; i < 16; i++)
		{
			for(j = 0; j < data->image_data.ac_tables[k].symbol_sizes[i];j++)
			{

				if((j + 1) % 2 == 0)
				{
					data->image_data.ac_tables[k].codes[l].right_child->code = data->image_data.ac_tables[k].symbols[i][j];
					data->image_data.ac_tables[k].codes[l].right_child->isLeaf = TRUE;
					data->image_data.ac_tables[k].codes[l].right_child->code_length = i + 1;
					printf("Right: [%d][%d]: %d\n", i + 1, j, data->image_data.ac_tables[k].codes[l].right_child->code);
					++l;
					isRight = TRUE;
				}
				else
				{
					isRight = FALSE;
					data->image_data.ac_tables[k].codes[l].code_length = i + 1;
					data->image_data.ac_tables[k].codes[l].left_child->code = data->image_data.ac_tables[k].symbols[i][j];
					data->image_data.ac_tables[k].codes[l].left_child->isLeaf = TRUE;
					data->image_data.ac_tables[k].codes[l].left_child->isFilled = TRUE;
					data->image_data.ac_tables[k].codes[l].left_child->code_length = i + 1;
					printf("Left:  [%d][%d]: %d\n", i + 1, j, data->image_data.ac_tables[k].codes[l].left_child->code);

				}
			}
			if(data->image_data.ac_tables[k].symbol_sizes[i] != 0 && !isRight)l++;
		}

		for(i = 0; i < l; i++)
		{

			printf("    %d\n\n", data->image_data.ac_tables[k].codes[i].code_length);
			printf("%d	", data->image_data.ac_tables[k].codes[i].left_child->code );
			printf("%d\n\n", 	 data->image_data.ac_tables[k].codes[i].right_child->code);
		}

		l = 0;
		k++;
	}
	isRight = FALSE;
	j = 0;
	k = 0;
	i = 1;

}
int main(int argc, char *argv[])
{
	//get the file to read from the commandline
	char * path = "./res/";
	char *args = malloc(sizeof(char)*(strlen(path) + strlen(argv[1])));
	strcat(args, path);
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
						for(i = 0; i < 4;i++)data.image_data.ac_tables[i].isFilled = FALSE;
						for(i = 0; i < 4;i++)data.image_data.dc_tables[i].isFilled = FALSE;
						parse_htables(&data);
						int k = 0;

						while(data.image_data.ac_tables[k].isFilled)
						{
							printf("Symbol Sizes[%d]: \n", k);
							for(i = 0; i < 16;i++)
							{
								printf("Amount of Symbols: %d of length %d\n", data.image_data.ac_tables[k].symbol_sizes[i], i + 1);
							}
							printf("AC Table[%d]\n", k);
							for(i = 0; i < data.image_data.ac_tables[k].rows;i++)
							{

								for(j = 0; j < data.image_data.ac_tables[k].symbol_sizes[i];++j)
								{
									printf("Data of bit length : %d is : %d\n", i + 1 , data.image_data.ac_tables[k].symbols[i][j]);
								}
							}


							k++;
						}

						k = 0;

						while(data.image_data.dc_tables[k].isFilled)
						{
							printf("Symbol Sizes: \n");
							for(i = 0; i < 16;i++)
							{
								printf("Amount of Symbols: %d of length %d\n", data.image_data.dc_tables[k].symbol_sizes[i], i + 1);
							}
							printf("DC Table[%d]\n", k);
							for(i = 0; i < data.image_data.dc_tables[k].rows;i++)
							{

								for(j = 0; j < data.image_data.dc_tables[k].symbol_sizes[i];++j)
								{
									printf("Data of bit length : %d is : %d\n", i + 1 , data.image_data.dc_tables[k].symbols[i][j]);
								}

							}

							k++;
						}

						break;
					case DQT:
						printf("\nThis specifies one or more quantization tables: %#1x\n", (uint)c);
						parse_qtables(&data);
						j = 0;
						i = 0;
						counter = 0;
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
						scan_image(&data);
						printf("Start of Selection: %d\n", data.image_data.start_of_sel );
						printf("End of Selection: %d\n", data.image_data.end_of_sel );
						printf("Succesive Approximation high: %d\n", data.image_data.sa_high);
						printf("Succesive Approximation Low: %d\n", data.image_data.sa_low);
						printf("Huffman Data Length: %d\n", data.image_data.huffman_data_len);
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

	decomp_hdata(&data);

	return 0;
}
