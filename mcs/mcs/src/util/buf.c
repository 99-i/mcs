#include "util/buf.h"
#include <stdlib.h>
#include <string.h>
buf buf_construct()
{
	buf b;
	b.data = 0;
	b.size = 0;
	b.is_slice = false;
	return b;
}
buf buf_construct_data(u8* bytes, i32 size)
{
	buf b;
	b.size = size;
	b.data = mcsalloc(sizeof(u8) * size);
	memcpy(b.data, bytes, size);
	b.is_slice = false;
	return b;
}
void buf_append(buf* b, u8 byte)
{
	if (b->size == 0)
	{
		*b = buf_construct_data(&byte, 1);
		return;
	}
	b->size += 1;
	b->data = mcsrealloc(b->data, sizeof(u8) * b->size);
	b->data[b->size - 1] = byte;
}
void buf_append_buf(buf* b1, const buf b2)
{
	size_t original_size = b1->size;
	if (b1->size == 0)
	{
		*b1 = buf_construct_data(b2.data, b2.size);
		return;
	}
	if (b2.size == 0) return;

	b1->size += b2.size;
	b1->data = mcsrealloc(b1->data, sizeof(u8) * b1->size);
	memcpy(b1->data + original_size, b2.data, b2.size);
}
buf buf_slice_from_buf(buf b1, i32 low, i32 high)
{
	buf temp;
	temp = buf_construct_data(b1.data + low, high - low);
	temp.is_slice = true;
	return temp;
}
void buf_destroy(buf* b)
{
	if (b->is_slice) return;

	free(b->data);
	b->size = 0;
	b->data = 0;
}
