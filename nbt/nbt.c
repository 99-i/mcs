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
uint8_t next(read_stream_t *stream);
uint8_t current(read_stream_t *stream);
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

void *nbt_malloc(size_t block_size);


nbt *read_nbt(const uint8_t *bytes, size_t size)
{
	read_stream_t stream;
	stream.bytes = bytes;
	stream.current = 0;
	stream.total_size = size;

	nbt *o;
	
	o->main_compound = read_tag(&stream, true);



	return o;
}


struct nbt_tag *read_tag(read_stream_t *stream, bool has_name)
{

	switch(current(stream))
	{
		case TAG_BYTE:
		{
			
			break;
		}
		case TAG_SHORT:
		{

			break;
		}
		case TAG_INT:
		{

			break;
		}
		case TAG_LONG:
		{

			break;
		}

		case TAG_FLOAT:
		{

			break;
		}
		case TAG_DOUBLE:
		{

			break;
		}
		case TAG_BYTE_ARRAY:
		{

			break;
		}
		case TAG_STRING:
		{
			break;
		}
		case TAG_LIST:
		{
			break;
		}
		case TAG_COMPOUND:
		{
			break;
		}
		case TAG_INT_ARRAY:
		{
			break;
		}
		case TAG_LONG_ARRAY:
		{
			break;
		}
		default:
		break;
	}
}



struct nbt_tag *read_byte(read_stream_t *stream, bool has_name)
{
	struct nbt_tag *tag = nbt_malloc(sizeof(struct nbt_tag));

	assert(current(stream) == TAG_BYTE);

	tag->has_name = has_name;
	tag->type = TAG_BYTE;
	

}
struct nbt_tag *read_short(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_int(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_long(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_float(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_double(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_byte_array(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_string(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_list(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_compound(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_int_array(read_stream_t *stream, bool has_name)
{

}
struct nbt_tag *read_long_array(read_stream_t *stream, bool has_name)
{

}




uint8_t current(read_stream_t *stream)
{
	return stream->bytes[stream->current];
}
uint8_t next(read_stream_t *stream)
{
	stream->current += 1;
	if(stream->current > stream->total_size)
	{
		printf("Error reading NBT.\n");
		exit(1);
	}
	return stream->bytes[stream->current];
}


void *nbt_malloc(size_t block_size)
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
