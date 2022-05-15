#undef uuid_t
#include <uv.h>
#include <stdio.h>
#include "mcs.h"
#include "mc_packets.h"
#include <stddef.h>

struct game_t *game;
uv_mutex_t lock;
int main()
{
	game = construct_game();
	uv_mutex_init(&lock);

	construct_slabs();

	init_game_loop();
	init_network_loop();

	uv_thread_t game_thread;
	uv_thread_t network_thread;

	uv_thread_create(&game_thread, run_game_loop, NULL);
	uv_thread_create(&network_thread, run_network_loop, NULL);

	uv_thread_join(&game_thread);
	uv_thread_join(&network_thread);

	uv_mutex_destroy(&lock);

	uv_loop_close(&game_loop);
	uv_loop_close(&network_loop);

	return 0;
}


