#pragma once
#include <stdint.h>
#include "util.h"
struct packet_metadata_t
{
	uint32_t packet_id;
	uint32_t length;
};

enum estate
{
	STATE_HANDSHAKING,
	STATE_STATUS,
	STATE_LOGIN,
	STATE_PLAY
};


enum efield_type
{
	FT_VARINT,
	FT_UNSIGNED_SHORT,
	FT_UNSIGNED_BYTE,
	FT_LONG,
	FT_CHAT,
	FT_UUID,
	FT_IDENTIFIER,
	FT_BOOLEAN,
	FT_DOUBLE,
	FT_ANGLE,
	FT_INT,
	FT_SHORT,
	FT_POSITION,
	FT_BYTE,
	FT_FLOAT,
	FT_STRING_16,
	FT_STRING_20,
	FT_STRING_40,
	FT_STRING_255,
	FT_STRING_256,
	FT_STRING_384,
	FT_STRING_32500,
	FT_STRING_32767,
	FT_STRING,
	FT_VARLONG,
	FT_CONDITIONAL,
	FT_ARRAY_OF_SIZE
};



enum econdition
{
	CONDITION_EQUALS
};


struct slab_t
{
	char* name;
	uint32_t id;
	enum estate state;

	uint32_t num_fields;
	struct field_t
	{
		char* field_name;
		struct field_t
		{
			enum efield_type type;

			union
			{
				struct condition_t
				{
					enum econdition condition_type;
					enum efield_type result;

					union
					{
						struct
						{
							char* field_one;
							char* field_two;
						} equals;
					} comparison;
				} condition;
				struct array_t
				{
					char* size_field;
					enum efield_type type_of_size_field;
					enum efield_type type_of_elements;
				} array;
			} field;
		} fieldinfo;
	}*fields;
};

struct packet_metadata_t get_packet_metadata(uint32_t data_length, uint8_t* data);

struct packet_t
{
	char* type;

};

