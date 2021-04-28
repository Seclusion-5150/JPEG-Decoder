#include "linkedlist.h"
#include "jpeh.h"
//#define create_array(data) _Generic((data), char*: create_array_string, struct linked_list_string*: create_array_llstring, char: create_array_char)(data);
//#define print_list(data) _Generic((data), char**: print_list_string, struct linked_list_string**: print_list_llstring)(data);
//#define add_item(list, data) _Generic((list, data), char*: add_item_string, struct linked_list_string*: add_item_llstring, char: add_item_char)(list, data);


char ** create_array_string(char *data)
{
  char ** array = (char**)malloc(sizeof(char*));
  array[0] = (char*)malloc(sizeof(char) * strlen(data));
  array[0] = data;

  return array;
}
char * create_array_char(char data)
{
  char * array = (char*)malloc(sizeof(char));
  array[0] = data;

  return array;
}
struct linked_list_string ** create_array_llstring(struct linked_list_string *data)
{
  struct linked_list_string ** array = (struct linked_list_string**)malloc(sizeof(struct linked_list_string*));
  array[0] = (struct linked_list_string*)malloc(sizeof(struct linked_list_char) * 1);
  array[0] = data;

  return array;
}
struct quantization_table * create_array_qtable(byte data[8][8])
{
  struct linked_list_string * array = (struct linked_list_string*)malloc(sizeof(struct linked_list_string));
  array->qtable = data;

  return array;
}
char * add_item_char(char *array, char c, int array_length)
{
  char *new_array = (char*)realloc(array, sizeof(char) * (++array_length));
  if(new_array != NULL) new_array[array_length - 1] = c;
  else
  {
    new_array = NULL;
    printf("Error Array is NULL\n");
  }
  return new_array;
}

char ** add_item_string(char ** array, char * string, int array_length)
{
  char ** new_array = realloc(array, sizeof(char*) * (++array_length));
  new_array[array_length - 1] = string;
  return new_array;
}

struct linked_list_string ** add_item_llstring(struct linked_list_string ** array, struct linked_list_string * string, int array_length)
{
  struct linked_list_string ** new_array = (struct linked_list_string**)realloc(array, ++array_length);
  new_array[array_length - 1] = string;
  return new_array;
}
struct quantization_table * add_item_qtable(struct quantization_table * array, byte data[8][8] , int array_length)
{
  struct quantization_table * new_array = (struct quantization_table*)realloc(array, ++array_length);
  new_array[array_length - 1] = data;
  return new_array;
}
void print_list_char(char * array, int array_length)
{
  int i, j;
  for(i = 0; i < array_length; i++)
  {
    printf("%c", array[i]);
  }
  printf("\n");
}

void print_list_string(char ** array, int array_length)
{
  int i, j;
  for(i = 0; i < array_length; i++)
  {
    printf("%s\n", array[i]);
  }
}
void print_list_llstring(struct linked_list_string ** array, int array_length)
{
  int i, j;
  for(i = 0; i < array_length; i++)
  {
    display_list(array[i]);
  }
  printf("\n");
}
