#pragma once
#include "util/def.h"
#include "util/string.h"
#include "util/client_array.h"
#include "util/world_array.h"
#include "util/slab_field_array.h"
#include <uv.h>
#undef uuid_t
struct position_t
{
	i32 x;
	i16 y;
	i32 z;
};
struct uuid_t
{
	u64 high;
	u64 low;
};

typedef struct vector3f_t
{
	float x;
	float y;
	float z;
} vector3f;

struct world_t
{
	int i;
	//TODO
};
struct entity_player_t;
struct game_t;


enum estate
{
	STATE_HANDSHAKING,
	STATE_STATUS,
	STATE_LOGIN,
	STATE_PLAY
};

struct client_t
{
	uv_tcp_t *socket;
	enum estate state;
	struct entity_player_t *player;
};

struct entity_player_t
{
	str username;
	str display_name;
	vector3f location;
	vector3f velocity;
	vector3f acceleration;
};

struct server_t
{
	client_array clients;
};

struct game_t
{
	struct server_t *server;
	world_array worlds;
	uint32_t tick_count;
};
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
struct wraparound_t
{
	uint32_t cutoff;
	uint32_t first_size;
	uint32_t second_size;
};
