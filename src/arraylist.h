#include "jpg_dec.h"


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
char * create_array_byte(byte data)
{
  byte * array = (byte*)malloc(sizeof(byte));
  array[0] = data;

  return array;
}
char * add_item_char(char *array, char data, int array_length)
{
  char *new_array = (char*)realloc(array, sizeof(char) * (++array_length));
  if(new_array != NULL) new_array[array_length - 1] = data;
  else
  {
    new_array = NULL;
    printf("Error Array is NULL\n");
  }
  return new_array;
}
char * add_item_byte(char *array, byte data, int array_length)
{
  byte *new_array = (byte*)realloc(array, sizeof(byte) * (++array_length));
  if(new_array != NULL) new_array[array_length - 1] = data;
  else
  {
    new_array = NULL;
    printf("Error Array is NULL\n");
  }
  return new_array;
}

char ** add_item_string(char ** array, char * data, int array_length)
{
  char ** new_array = realloc(array, sizeof(char*) * (++array_length));
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
void print_list_byte(byte * array, int array_length)
{
  int i, j;
  for(i = 0; i < array_length; i++)
  {
    printf("%u", array[i]);
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
