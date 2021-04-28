#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#define search_list(list, data) _Generic((list, data), int: search_list_int, char: search_list_char, double: search_list_double, long: search_list_long, float: search_list_float, unsigned int: search_list_uint, unsigned char: search_list_uchar, unsigned long: search_list_ulong, char *: search_list_string)(list,data);
#define del(list, data)         _Generic((list,data), int: del_int, char: del_char, double: del_double, long: del_long, float: del_float, unsigned int: del_uint, unsigned char: del_uchar, unsigned long: del_ulong, char*: del_string)(list,data);
#define create_node(data)       _Generic((data), int: create_node_int, double: create_node_double, float: create_node_float, char: create_node_char, unsigned int: create_node_uint, unsigned long: create_node_ulong, unsigned char: create_node_uchar, char*: create_node_string)(data);

struct linked_list_int
{
	int data;
	struct linked_list_int *next;

};

struct linked_list_char
{
 	char data;
	struct linked_list_char *next;
};

struct linked_list_double
{
	double data;
	struct linked_list_double *next;
};

struct linked_list_float
{
	float data;
	struct linked_list_float *next;
};

struct linked_list_long
{
	long data;
	struct linked_list_long *next;
};

struct linked_list_ulong
{
	unsigned long data;
	struct linked_list_ulong *next;
};

struct linked_list_uint
{
	unsigned int data;
	struct linked_list_uint *next;
};

struct linked_list_uchar
{
	unsigned char data;
	struct linked_list_uchar *next;
};

struct linked_list_string
{
	char* data;
	struct linked_list_string *next;
};

#define display_list(list) _Generic ((list), struct linked_list_int*: display_list_int, struct linked_list_double*: display_list_double, struct linked_list_float*: display_list_float, struct linked_list_char*: display_list_char, struct linked_list_uint*: display_list_uint, struct linked_list_ulong*: display_list_ulong, struct linked_list_uchar*: display_list_uchar, struct linked_list_string*: display_list_string)(list);

struct linked_list_string *create_node_string(char* data)
{
	struct linked_list_string *node = (struct linked_list_string*)malloc(sizeof(struct linked_list_string));
	node->data = data;
	node->next = NULL;
	return node;
}

struct linked_list_int *create_node_int(int data)
{
	struct linked_list_int *node = (struct linked_list_int*)malloc(sizeof(struct linked_list_int));
	node->data = data;
	node->next = NULL;
	return node;
}
struct linked_list_double *create_node_double(double data)
{
	struct linked_list_double * node = (struct linked_list_double*)malloc(sizeof(struct linked_list_double));
	node->data = (double)data;
	node->next = NULL;
	return node;
}
struct linked_list_long *create_node_long(long data)
{
	struct linked_list_long * node = (struct linked_list_long*)malloc(sizeof(struct linked_list_long));

	node->data = (long)data;
	node->next = NULL;
	return node;
}
struct linked_list_char *create_node_char(char data)
{
	struct linked_list_char *node = (struct linked_list_char*)malloc(sizeof(struct linked_list_char));

	node->data = (char)data;
	node->next = NULL;
	return node;
}

struct linked_list_float *create_node_float(float data)
{
	struct linked_list_float * node = (struct linked_list_float*)malloc(sizeof(struct linked_list_float));

	node->data = data;
	node->next = NULL;
	return node;
}
struct linked_list_uint *create_node_uint(unsigned int data)
{
  struct linked_list_uint * node = (struct linked_list_uint*)malloc(sizeof(struct linked_list_uint));

	node->data = (unsigned int)data;
	node->next = NULL;
	return node;
}
struct linked_list_ulong *create_node_ulong(unsigned long data)
{
	struct linked_list_ulong * node = (struct linked_list_ulong*)malloc(sizeof(struct linked_list_ulong));

	node->data = (unsigned long)data;
	node->next = NULL;
	return node;
}
struct linked_list_uchar *create_node_uchar(unsigned char data)
{
	struct linked_list_uchar * node = (struct linked_list_uchar*)malloc(sizeof(struct linked_list_uchar));

	node->data = (unsigned char)data;
	node->next = NULL;
	return node;
}
void display_list_string(struct linked_list_string *list)
{
	struct linked_list_string *temp = list;
	while(temp != NULL)
	{
 		printf("%s ", temp->data);
		temp = temp->next;
	}
	printf("\n");
}

void display_list_int(struct linked_list_int *list)
{
	struct linked_list_int *temp = list;
	while(temp != NULL)
	{
 		printf("%d ", temp->data);
		temp = temp->next;
	}
	printf("\n");
}
void display_list_double(struct linked_list_double *list)
{
	struct linked_list_double *temp = list;
	while(temp != NULL)
	{
 		printf("%f ", temp->data);
		temp = temp->next;
	}
	printf("\n");

}
void display_list_float(struct linked_list_float *list)
{
	struct linked_list_float *temp = list;
	while(temp != NULL)
	{
 		printf("%f ", temp->data);
		temp = temp->next;
	}
	printf("\n");
}
void display_list_char(struct linked_list_char *list)
{
	struct linked_list_char *temp = list;
	while(temp != NULL)
	{
 		printf("%c ", temp->data);
		temp = temp->next;
	}
	printf("\n");
}
void display_list_long(struct linked_list_long *list)
{
	struct linked_list_long *temp = list;
	while(temp != NULL)
	{
 		printf("%l ", temp->data);
		temp = temp->next;
	}
	printf("\n");
}
void display_list_uint(struct linked_list_uint *list)
{
	struct linked_list_uint *temp = list;
	while(temp != NULL)
	{
 		printf("%u ", temp->data);
		temp = temp->next;
	}
	printf("\n");
}
void display_list_ulong(struct linked_list_ulong *list)
{
	struct linked_list_ulong *temp = list;
	while(temp != NULL)
	{
 		printf("%lu ", temp->data);
		temp = temp->next;
	}
	printf("\n");
}
void display_list_uchar(struct linked_list_uchar *list)
{
	struct linked_list_uchar *temp = list;
	while(temp != NULL)
	{
 		printf("%u ", temp->data);
		temp = temp->next;
	}
	printf("\n");
}

struct linked_list_string* search_list_string(struct linked_list_string *list, char* data)
{
	while((list != NULL))
	{
		if(strcmp(list->data, data) == 0) return list;
		list = list->next;
	}
	return list;
}
struct linked_list_int* search_list_int(struct linked_list_int *list, int data)
{
	while((list != NULL))
	{
		list = list->next;
		if(list->data == data)
		{
			return list;
		}
	}
	return list;
}
struct linked_list_char *search_list_char(struct linked_list_char *list, char data)
{
	while((list != NULL))
	{
		list = list->next;
		if(list->data == data)
		{
			return list;
		}
	}
	return list;
}
struct linked_list_double *search_list_double(struct linked_list_double *list, double data)
{
	while((list != NULL))
	{
		list = list->next;
		if(list->data == data)
		{
			return list;
		}
	}
	return list;
}
struct linked_list_long *search_list_long(struct linked_list_long *list, long data)
{
	while((list != NULL))
		{
			list = list->next;
			if(list->data == data)
			{
				return list;
			}
		}
		return list;
}
struct linked_list_float *search_list_float(struct linked_list_float *list, float data)
{
	while((list != NULL))
	{
		list = list->next;
		if(list->data == data)
		{
			return list;
		}
	}
	return list;
}
struct linked_list_uint *search_list_uint(struct linked_list_uint *list, unsigned int data)
{
	while((list != NULL))
	{
		list = list->next;
		if(list->data == data)
		{
			return list;
		}
	}
	return list;
}

struct linked_list_ulong *search_list_ulong(struct linked_list_ulong *list, unsigned long data)
{
	while((list != NULL))
	{
		list = list->next;
		if(list->data == data)
		{
			return list;
		}
	}
	return list;
}
struct linked_list_uchar *search_list_uchar(struct linked_list_uchar *list, unsigned char data)
{

	while((list != NULL))
	{
		list = list->next;
		if(list->data == data)
		{
			return list;
		}
	}
	return list;
}
void del_string(struct linked_list_string *list, char* data)
{
	struct linked_list_string* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;

}

void del_int(struct linked_list_int *list, int data)
{
	struct linked_list_int* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;

}
void del_double(struct linked_list_double *list, double data)
{
	struct linked_list_double* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;

}
void del_float(struct linked_list_float *list, float data)
{
	struct linked_list_float* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;

}
void del_long(struct linked_list_long *list, long data)
{
	struct linked_list_long* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;
}
void del_char(struct linked_list_char *list, char data)
{
	struct linked_list_char* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;
}
void del_uint(struct linked_list_uint *list, unsigned int data)
{
	struct linked_list_uint* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;

}
void del_ulong(struct linked_list_ulong *list, unsigned long data)
{
	struct linked_list_ulong* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;

}
void del_uchar(struct linked_list_uchar *list, unsigned char data)
{
	struct linked_list_uchar* node = search_list(list, data);
	list = node->next;
	free(node);
	node = NULL;
}
