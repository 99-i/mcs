#include "mcs.h"

struct server_t *construct_server(void)
{
	struct server_t *server;
	server = malloc(sizeof(struct server_t));
	server->client_count = 0;

	return server;
}

struct game_t *construct_game(void)
{
	struct game_t *g;

	g = malloc(sizeof(struct game_t));
	g->tick_count = 0;
	g->world_count = 0;
	g->server = construct_server();

	return g;
}
