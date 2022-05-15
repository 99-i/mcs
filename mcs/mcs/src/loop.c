#include "mcs.h"
#include <stdio.h>
#include <assert.h>
#include "util/string.h"

uv_loop_t main_loop;

static uv_timer_t timer;

static void timer_callback(uv_timer_t* handle)
{
	assert(handle == &timer);
}
void init_main_loop(void)
{
	uv_loop_init(&main_loop);
	uv_timer_init(&main_loop, &timer);
	uv_timer_start(&timer, timer_callback, 0, 1000 / 40);
}

void run_main_loop(void* data)
{
	uv_run(&main_loop, UV_RUN_DEFAULT);
}
