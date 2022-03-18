#include <uv.h>
#include <stdio.h>
#include "mcs.h"
#include "mc_packets.h"
#include <stddef.h>

int main()
{
	construct_slabs();

	init_game_loop();
	init_main_loop();
	init_network_loop();

	uv_thread_t game_thread;
	uv_thread_t main_thread;
	uv_thread_t network_thread;

	uv_thread_create(&main_thread, run_main_loop, NULL);
	uv_thread_create(&game_thread, run_game_loop, NULL);
	uv_thread_create(&network_thread, run_network_loop, NULL);

	uv_thread_join(&main_thread);
	uv_thread_join(&game_thread);
	uv_thread_join(&network_thread);
	return 0;
}
