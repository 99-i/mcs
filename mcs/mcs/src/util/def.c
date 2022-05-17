#include "util/def.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int num_mallocs = 0;
long total_allocated_memory = 0;
void* mcsalloc_(i32 s, const char* func)
{
	if(!strcmp("map_set", func))
	{
		int j = 2;
		j++;
	}
	void* m = malloc(s);
	if(m == 0)
	{
		assert(false && "we have a problem.");
		exit(1);
	}
	return m;
}

void *mcsrealloc(void *block, i32 s)
{
	void* m = realloc(block, s);
	if(m == 0)
	{
		assert(false && "we have a problem.");
		exit(1);
	}
	return m;
}

void mcsfree_(void *block, const char* func)
{
	num_mallocs--;
	total_allocated_memory -= _msize(block);
	printf("just freed (a block the size of %lu) at %s. total is %ld.\n", _msize(block), func, total_allocated_memory);
	free(block);
}
