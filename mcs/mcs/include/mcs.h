#pragma once
#include <stdint.h>
#include <uv.h>
#include "message.h"

struct world_t;
struct entity_player_t;
struct game_t;

typedef struct vector3f_t
{
	float x;
	float y;
	float z;
} vector3f;


struct client_t
{
	uv_tcp_t* socket;
	struct entity_player_t* player;
};

struct entity_player_t
{
	char username[16];
	char display_name[32];
	vector3f location;
	vector3f velocity;
	vector3f acceleration;
};

struct server_t
{
	uint32_t client_count;
	struct client_t* players;
};

struct game_t
{
	struct server_t* server;
	uint32_t world_count;
	struct world_t* worlds;
	uint32_t tick_count;
};





extern uv_loop_t main_loop;
extern uv_loop_t game_loop;
extern uv_loop_t network_loop;

void init_game_loop(void);
void run_game_loop(void* data);

void init_main_loop(void);
void run_main_loop(void* data);

void init_network_loop(void);
void run_network_loop(void* data);
