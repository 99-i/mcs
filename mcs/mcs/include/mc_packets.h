#pragma once
#undef uuid_t
#include <stdint.h>
#include "util/slab_field_array.h"
#include "util/string.h"
#include "util/buf.h"
#include "util/map.h"
#include "util/slab_array.h"

struct packet_metadata_t
{
	uint8_t packet_id;
	uint16_t length;
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

struct packet_metadata_t get_packet_metadata(buf b);

struct packet_t
{
	enum epacket_direction direction;
	str type;
	map map;
};

enum efield_type
{
	FT_VARINT,
	FT_UNSIGNED_SHORT,
	FT_UNSIGNED_BYTE,
	FT_LONG,
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
};
struct field_t
{
	str field_name;
	enum efield_type type;
};
struct slab_t
{
	str name;
	uint32_t id;
	enum estate state;
	enum epacket_direction direction;
	slab_field_array fields;
};

struct slabinfo_t
{
	slab_array slabs;
};
extern struct slabinfo_t slabinfo;

void slurp_file_to_cstr(const char* file_name, char** cstr);

void construct_slabs(void);

struct wraparound_t
{
	uint32_t cutoff;
	uint32_t first_size;
	uint32_t second_size;
};
bool should_wraparound(buf b, struct wraparound_t *cutoff);
bool create_serverbound_packet(buf b, enum estate state, struct packet_t *packet);

struct packet_t* construct_clientbound_packet(str packet_type, ...);


void packet_free(struct packet_t* packet);

buf make_varint(i32 varint);
buf make_u16(u16 unsigned_short);
buf make_u8(u8 unsigned_byte);
buf make_i64(i64 l);
buf make_uuid(struct uuid_t uuid);
buf make_boolean(bool boolean);
buf make_double(double d);
buf make_angle(u8 angle);
buf make_i32(i32 integer);
buf make_i16(i16 s);
buf make_position(struct position_t pos);
buf make_i8(i8 byte);
buf make_float(float f);
buf make_string(str str);
buf make_varlong(i64 varlong);
