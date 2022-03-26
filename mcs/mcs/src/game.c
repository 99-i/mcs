#include "mcs.h"
#include <stdio.h>
#include <assert.h>

uv_loop_t game_loop;

static uv_timer_t timer;

static void timer_callback(uv_timer_t *handle)
{
	assert(handle == &timer);
}
void init_game_loop(void)
{
	uv_loop_init(&game_loop);
	uv_timer_init(&game_loop, &timer);
	uv_timer_start(&timer, timer_callback, 0, 1000 / 40);
}

void run_game_loop(void *data)
{
	uv_run(&game_loop, UV_RUN_DEFAULT);
}

struct client_t *game_init_client(uv_tcp_t *socket)
{
	struct client_t *client;
	uv_mutex_lock(&lock);
	client = malloc(sizeof(struct client_t));
	client->socket = socket;
	client->state = STATE_HANDSHAKING;
	game->server->client_count += 1;
	game->server->clients = realloc(game->server->clients, sizeof(struct client_t) * game->server->client_count);
	game->server->clients[game->server->client_count - 1] = client;
	uv_mutex_unlock(&lock);
	return client;
}
