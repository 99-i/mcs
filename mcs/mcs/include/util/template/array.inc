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
	i32 capacity;
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
	array.capacity = size;
	array.size = 0;
	array.fields = mcsalloc(sizeof(ARRAY_TYPE) * size);
	return array;
}
void CAT(ARRAY_STRUCT_NAME, _append)(ARRAY_STRUCT_NAME* array, ARRAY_TYPE field)
{
	if (array->capacity == array->size)
	{
		array->capacity += 1;
		if (array->capacity == 1)
		{
			array->fields = mcsalloc(sizeof(ARRAY_TYPE) * 1);
		}
		else
		{
			array->fields = mcsrealloc(array->fields, array->size * sizeof(ARRAY_TYPE));
		}
	}
	array->fields[array->size] = field;
	array->size++;
}
void CAT(ARRAY_STRUCT_NAME, _destroy)(ARRAY_STRUCT_NAME* array)
{
	array->size = 0;
	array->capacity = 0;
	free(array->fields);
	array->fields = 0;
}

void CAT(ARRAY_STRUCT_NAME, _remove)(ARRAY_STRUCT_NAME* array, i32 index)
{

	memmove(array->fields + index, array->fields + index + 1, (array->size - 1 - index) * sizeof(ARRAY_TYPE));
	array->size--;
}
#endif

#endif
