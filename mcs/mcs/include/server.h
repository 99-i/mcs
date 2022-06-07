#pragma once
#include "mcs.h"
#include "util/string.h"

void server_send_packet(struct client_t *client, struct packet_t *packet);

typedef void (*get_player_uuid_callback)(struct uuid_t, void*);

typedef void (*get_request_callback)(str, void*);

void get_request(str url, get_request_callback callback, void* data);
void server_get_player_uuid(str username, get_player_uuid_callback callback, void* data);
