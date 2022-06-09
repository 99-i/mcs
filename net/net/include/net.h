#pragma once
#include <stdint.h>

struct net_get_t
{
	size_t size;
	char* data;
};

void net_init(void);

typedef void(*net_get_cb)(struct net_get_t* res);



