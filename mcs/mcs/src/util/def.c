#include "util/def.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

void* mcsalloc(i32 s)
{
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

void mcsfree(void *block)
{
	free(block);
}
