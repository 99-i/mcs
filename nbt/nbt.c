#include "nbt.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
typedef struct read_stream_s
{
	const uint8_t *bytes;
	size_t total_size;
	size_t current;
} read_stream_t;
static uint8_t next(read_stream_t *stream);
static uint8_t current(read_stream_t *stream);
static void inc(read_stream_t *stream);


struct nbt_tag *read_tag(read_stream_t *stream, bool has_name);

struct nbt_tag *read_byte(read_stream_t *stream, bool has_name);
struct nbt_tag *read_short(read_stream_t *stream, bool has_name);
struct nbt_tag *read_int(read_stream_t *stream, bool has_name);
struct nbt_tag *read_long(read_stream_t *stream, bool has_name);
struct nbt_tag *read_float(read_stream_t *stream, bool has_name);
struct nbt_tag *read_double(read_stream_t *stream, bool has_name);
struct nbt_tag *read_byte_array(read_stream_t *stream, bool has_name);
struct nbt_tag *read_string(read_stream_t *stream, bool has_name);
struct nbt_tag *read_list(read_stream_t *stream, bool has_name);
struct nbt_tag *read_compound(read_stream_t *stream, bool has_name);
struct nbt_tag *read_int_array(read_stream_t *stream, bool has_name);
struct nbt_tag *read_long_array(read_stream_t *stream, bool has_name);

static void *nbt_malloc(size_t block_size);

nbt *read_nbt(const uint8_t *bytes, size_t size)
{
	read_stream_t stream;
	stream.bytes = bytes;
	stream.current = -1;
	stream.total_size = size;

	nbt *o = nbt_malloc(sizeof(nbt));
	
	o->main_compound = read_tag(&stream, true);



	return o;
}


struct nbt_tag *read_tag(read_stream_t *stream, bool has_name)
{

	switch(next(stream))
	{
		case TAG_BYTE:
		{
			return read_byte(stream, has_name);
			break;
		}
		case TAG_SHORT:
		{
			return read_short(stream, has_name);
			break;
		}
		case TAG_INT:
		{
			return read_int(stream, has_name);
			break;
		}
		case TAG_LONG:
		{
			return read_long(stream, has_name);
			break;
		}
		case TAG_FLOAT:
		{
			return read_float(stream, has_name);
			break;
		}
		case TAG_DOUBLE:
		{
			return read_double(stream, has_name);
			break;
		}
		case TAG_BYTE_ARRAY:
		{
			return read_byte_array(stream, has_name);
			break;
		}
		case TAG_STRING:
		{
			return read_string(stream, has_name);
			break;
		}
		case TAG_LIST:
		{
			return read_list(stream, has_name);
			break;
		}
		case TAG_COMPOUND:
		{
			return read_compound(stream, has_name);
			break;
		}
		case TAG_INT_ARRAY:
		{
			return read_int_array(stream, has_name);
			break;
		}
		case TAG_LONG_ARRAY:
		{
			return read_long_array(stream, has_name);
			break;
		}
		default:
			assert(false && "unreachable.");
			break;
	}

}



struct nbt_tag *read_byte(read_stream_t *stream, bool has_name)
{
	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_BYTE);

	tag->type = TAG_BYTE;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}

	tag->tag_byte = next(stream);

	return tag;
}
struct nbt_tag *read_short(read_stream_t *stream, bool has_name)
{
	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_SHORT);

	tag->type = TAG_SHORT;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint16_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}
	int16_t s = 0;
	s |= (int16_t) next(stream) << 8;
	s |= next(stream);


	tag->tag_short = s;

	return tag;
}
struct nbt_tag *read_int(read_stream_t *stream, bool has_name)
{
	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_INT);

	tag->type = TAG_INT;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}

	int32_t i = 0;
	i |= (int32_t) next(stream) << 24;
	i |= (int32_t) next(stream) << 16;
	i |= (int32_t) next(stream) << 8;
	i |= next(stream);

	tag->tag_int = i;

	return tag;

}
struct nbt_tag *read_long(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_LONG);

	tag->type = TAG_LONG;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}

	int64_t l = 0;
	l |= (int64_t) next(stream) << 56;
	l |= (int64_t) next(stream) << 48;
	l |= (int64_t) next(stream) << 40;
	l |= (int64_t) next(stream) << 32;
	l |= (int64_t) next(stream) << 24;
	l |= (int64_t) next(stream) << 16;
	l |= (int64_t) next(stream) << 8;
	l |= next(stream);

	tag->tag_long = l;


	return tag;

}
struct nbt_tag *read_float(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_FLOAT);

	tag->type = TAG_FLOAT;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}

	float f = 0;
	int32_t i = 0;
	i |= (int32_t) next(stream) << 24;
	i |= (int32_t) next(stream) << 16;
	i |= (int32_t) next(stream) << 8;
	i |= next(stream);

	*((int32_t*) &f) = i;

	tag->tag_float = f;

	return tag;
}
struct nbt_tag *read_double(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_DOUBLE);

	tag->type = TAG_DOUBLE;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}

	double d = 0;
	int64_t l = 0;
	l |= (int64_t) next(stream) << 56;
	l |= (int64_t) next(stream) << 48;
	l |= (int64_t) next(stream) << 40;
	l |= (int64_t) next(stream) << 32;
	l |= (int64_t) next(stream) << 24;
	l |= (int64_t) next(stream) << 16;
	l |= (int64_t) next(stream) << 8;
	l |= next(stream);

	*((int64_t*) &d) = l;

	tag->tag_double= d;

	return tag;


}
struct nbt_tag *read_byte_array(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_BYTE_ARRAY);

	tag->type = TAG_BYTE_ARRAY;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}
	tag->tag_byte_array.size = 0;
	tag->tag_byte_array.size |= (int32_t) next(stream) << 24;
	tag->tag_byte_array.size |= (int32_t) next(stream) << 16;
	tag->tag_byte_array.size |= (int32_t) next(stream) << 8;
	tag->tag_byte_array.size |= next(stream);
	
	tag->tag_byte_array.bytes = nbt_malloc(tag->tag_byte_array.size);

	for(size_t i = 0; i < tag->tag_byte_array.size; i++)
	{
		tag->tag_byte_array.bytes[i] = next(stream);
	}

	return tag;
}
struct nbt_tag *read_string(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_STRING);

	tag->type = TAG_STRING;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}

	uint16_t str_size = 0;
	str_size |= (uint16_t) next(stream) << 8;
	str_size |= next(stream);

	//todo: modified utf-8 reader

}

struct nbt_tag *read_list(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_LIST);

	tag->type = TAG_LIST;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}
}
struct nbt_tag *read_compound(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_COMPOUND);

	tag->type = TAG_COMPOUND;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}
}
struct nbt_tag *read_int_array(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_INT_ARRAY);

	tag->type = TAG_INT_ARRAY;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}
}
struct nbt_tag *read_long_array(read_stream_t *stream, bool has_name)
{

	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_LONG_ARRAY);


	tag->type = TAG_LONG_ARRAY;

	if(has_name)
	{
		uint16_t name_size = 0;
		name_size |= (((uint32_t) next(stream)) << 8);
		name_size |= next(stream);
		tag->name = nbt_malloc((size_t) name_size + 1);
		for(size_t i = 0; i < name_size; i++)
		{
			tag->name[i] = next(stream);
		}
		tag->name[name_size] = 0;
	}
	else
	{
		tag->name = 0;
	}
}




static uint8_t current(read_stream_t *stream)
{
	return stream->bytes[stream->current];
}
static uint8_t next(read_stream_t *stream)
{
	stream->current += 1;
	if(stream->current > stream->total_size)
	{
		printf("Error reading NBT.\n");
		exit(1);
	}
	return stream->bytes[stream->current];
}
static void inc(read_stream_t *stream)
{
	stream->current += 1;
}
static void *nbt_malloc(size_t block_size)
{
	void *b = malloc(block_size);
	if(b == 0)
	{
		printf("Error: couldn't allocate memory for NBT.\n");
		exit(1);
	}

	for(size_t i = 0; i < block_size; i++)
	{
		((char*) b)[i] = 0;
	}
	

	return b;


}

