#include "mcs.h"
#include <stdio.h>
#include <assert.h>

uv_loop_t game_loop;

static uv_timer_t timer;

static void timer_callback(uv_timer_t* handle)
{
	assert(handle == &timer);
}
void init_game_loop(void)
{
	uv_loop_init(&game_loop);
	uv_timer_init(&game_loop, &timer);
	uv_timer_start(&timer, timer_callback, 0, 1000 / 40);
}

void run_game_loop(void* data)
{
	uv_run(&game_loop, UV_RUN_DEFAULT);
}
