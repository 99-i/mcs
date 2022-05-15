#include "mcs.h"

struct server_t *construct_server(void)
{
	struct server_t *server;
	server = malloc(sizeof(struct server_t));
	server->clients = client_array_construct(0);
	return server;
}

struct game_t *construct_game(void)
{
	struct game_t *g;

	g = malloc(sizeof(struct game_t));
	g->tick_count = 0;
	g->worlds = world_array_construct(0);
	g->server = construct_server();

	return g;
}
