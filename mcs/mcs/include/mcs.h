#pragma once
#include <stdint.h>
#include <uv.h>
#include "game.h"
#include "util/world_array.h"
#include "util/client_array.h"
#include "types.h"

extern uv_mutex_t lock;
struct server_t *construct_server(void);

extern uv_loop_t game_loop;
extern uv_loop_t network_loop;

extern struct game_t *game;


void init_network_loop(void);
void run_network_loop(void *data);
