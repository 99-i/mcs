#pragma once
#include <stdint.h>
#include "def.h"
#include <stdbool.h>
typedef struct buf
{
	i32 size;
	u8* data;
	bool is_slice;
} buf;

buf buf_construct();
buf buf_construct_data(u8* bytes, i32 size);
void buf_append(buf* b, u8 byte);
void buf_append_buf(buf* b1, const buf b2);
buf buf_slice_from_buf(buf b1, i32 low, i32 high);
void buf_destroy(buf* b);
