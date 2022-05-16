#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util/def.h"
#define _CAT(x, y) x ## y
#define CAT(x, y) _CAT(x, y)
#define ARRAY_STRUCT_NAME CAT(ARRAY_NAME, _array)
#ifdef ARRAY_DEFINITIONS
typedef struct ARRAY_STRUCT_NAME
{
	i32 size;
	ARRAY_TYPE *fields;
} ARRAY_STRUCT_NAME;

ARRAY_STRUCT_NAME CAT(ARRAY_STRUCT_NAME, _construct)(i32 size);
void CAT(ARRAY_STRUCT_NAME, _append)(ARRAY_STRUCT_NAME *array, ARRAY_TYPE field);
void CAT(ARRAY_STRUCT_NAME, _destroy)(ARRAY_STRUCT_NAME *array);
void CAT(ARRAY_STRUCT_NAME, _remove)(ARRAY_STRUCT_NAME *array, i32 index);
#else 

#ifdef ARRAY_IMPLEMENTATION
ARRAY_STRUCT_NAME CAT(ARRAY_STRUCT_NAME, _construct)(i32 size)
{
	ARRAY_STRUCT_NAME array;
	array.size = 0;
	array.fields = 0;
	return array;
}
void CAT(ARRAY_STRUCT_NAME, _append)(ARRAY_STRUCT_NAME* array, ARRAY_TYPE field)
{
	array->size++;
	if (array->size == 1)
	{
		array->fields = mcsalloc(sizeof(ARRAY_TYPE) * array->size);
	}
	else
	{
		array->fields = mcsrealloc(array->fields, array->size * sizeof(ARRAY_TYPE));
	}
	array->fields[array->size - 1] = field;
}
void CAT(ARRAY_STRUCT_NAME, _destroy)(ARRAY_STRUCT_NAME* array)
{
	array->size = 0;
	mcsfree(array->fields);
	array->fields = 0;
}

void CAT(ARRAY_STRUCT_NAME, _remove)(ARRAY_STRUCT_NAME* array, i32 index)
{
	i32 i;
	for (i = index - 1; i < index -1; i++)
	{
		array->fields[i] = array->fields[i+1];
	}
	array->size--;
	if(array->size != 0)
	{
		array->fields = mcsrealloc(array->fields, array->size * sizeof(ARRAY_TYPE));
	}
}
#endif

#endif
