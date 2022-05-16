#include "util/def.h"
#include <stdlib.h>
#include <assert.h>
void *mcsalloc(u64 s)
{
	void* m = malloc(s);
	if(m == 0)
	{
		assert(false && "we have a problem.");
		exit(1);
	}
	return m;
}

void *mcsrealloc(void *block, u64 s)
{
	void* m = realloc(block, s);
	if(m == 0)
	{
		assert(false && "we have a problem.");
		exit(1);
	}
	return m;
}
