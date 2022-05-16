#pragma once
#include "world.h"
#include "slab.h"
#include "util/def.h"
#include "util/string.h"
#include "util/client_array.h"

#include "util/world_array.h"
#include "util/slab_field_array.h"
#include <uv.h>
#undef uuid_t

typedef struct vector3f_t
{
	float x;
	float y;
	float z;
} vector3f;

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
	i32 tick_count;
};
struct packet_metadata_t
{
	i32 packet_id;
	i32 length;
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
struct wraparound_t
{
	i32 cutoff;
	i32 first_size;
	i32 second_size;
};
