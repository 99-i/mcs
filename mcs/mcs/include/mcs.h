#pragma once
#include <stdint.h>
#include <uv.h>
#include "game.h"
#include "util/world_array.h"
#include "util/client_array.h"

extern uv_mutex_t lock;
struct world_t;
struct entity_player_t;
struct game_t;

typedef struct vector3f_t
{
	float x;
	float y;
	float z;
} vector3f;

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
struct server_t *construct_server(void);
struct game_t
{
	struct server_t *server;
	world_array worlds;
	uint32_t tick_count;
};



extern uv_loop_t game_loop;
extern uv_loop_t network_loop;

extern struct game_t *game;


void init_network_loop(void);
void run_network_loop(void *data);
