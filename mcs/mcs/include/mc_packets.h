#pragma once
#undef uuid_t
#include <stdint.h>
#include "util.h"


struct packet_metadata_t
{
	uint8_t packet_id;
	uint16_t length;
};



enum efield_type
{
	FT_VARINT,
	FT_UNSIGNED_SHORT,
	FT_UNSIGNED_BYTE,
	FT_LONG,
	FT_CHAT,
	FT_UUID,
	FT_BOOLEAN,
	FT_DOUBLE,
	FT_ANGLE,
	FT_INT,
	FT_SHORT,
	FT_POSITION,
	FT_BYTE,
	FT_FLOAT,
	FT_STRING,
	FT_VARLONG,
	FT_CONDITIONAL,
	FT_ARRAY_OF_SIZE
};



enum econdition
{
	CONDITION_EQUALS
};

enum epacket_direction
{
	SERVERBOUND,
	CLIENTBOUND
};
struct position_t
{
	int32_t x;
	int16_t y;
	int32_t z;
};
struct uuid_t
{
	uint64_t high;
	uint64_t low;
};

struct slab_t
{
	char *name;
	uint32_t id;
	enum estate state;
	enum epacket_direction direction;
	uint32_t num_fields;
	struct field_t
	{
		char *field_name;
		struct field_info_t
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
							char *field_one;
							char *field_two;
						} equals;
					} comparison;
				} condition;
				struct array_t
				{
					char *size_field;
					enum efield_type type_of_size_field;
					enum efield_type type_of_elements;
				} array;
			} field;
		} fieldinfo;
	}*fields;
};

struct packet_metadata_t get_packet_metadata(uint32_t data_length, uint8_t *data);

struct packet_t
{
	enum epacket_direction direction;
	char *type;
	map_t map;
};

struct slabinfo_t
{
	size_t num_slabs;
	struct slab_t* slabs;
};
extern struct slabinfo_t slabinfo;
void construct_slabs(void);

struct wraparound_t
{
	uint32_t cutoff;
	uint32_t first_size;
	uint32_t second_size;
};
bool should_wraparound(uint8_t *data, uint32_t data_size, struct wraparound_t *cutoff);

bool create_serverbound_packet(uint32_t data_size, uint8_t *data, enum estate state, struct packet_t *packet);

struct packet_t* construct_clientbound_packet(char* packet_type, ...);


void packet_free(struct packet_t* packet);

u8 *make_varint(i32 varint, size_t *length);
u8 *make_u16(u16 unsigned_short, size_t *length);
u8 *make_u8(u8 unsigned_byte, size_t *length);
u8 *make_i64(i64 l, size_t *length);
u8 *make_uuid(struct uuid_t uuid, size_t *length);
u8 *make_boolean(bool boolean, size_t *length);
u8 *make_double(double d, size_t *length);
u8 *make_angle(u8 angle, size_t *length);
u8 *make_i32(i32 integer, size_t *length);
u8 *make_i16(i16 s, size_t *length);
u8 *make_position(struct position_t pos, size_t *length);
u8 *make_i8(i8 byte, size_t *length);
u8 *make_float(float f, size_t *length);
u8 *make_string(const char *str, size_t *length);
u8 *make_varlong(i64 varlong, size_t *length);
