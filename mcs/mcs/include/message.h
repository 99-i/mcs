#pragma once
#include <uv.h>
#include "mc_packets.h"

enum emessage
{
	MESSAGE_SI_NEW_CONNECTION, MESSAGE_SI_DATA, MESSAGE_SI_CLOSE_CONNECTION,
	MESSAGE_SO_DATA, MESSAGE_SO_CLOSE_CONNECTION,
	MESSAGE_GI_SEND_PACKET, MESSAGE_GI_CLOSE_CLIENT,
	MESSAGE_GO_NEW_CLIENT, MESSAGE_GO_PACKET
};
struct message_t
{
	enum emessage type;
	void *data;
};

typedef struct message_si_new_connection_t
{
	uv_tcp_t *socket;
} message_si_new_connection;

typedef struct message_si_data_t
{
	uv_tcp_t *socket;
	uv_buf_t data;
} message_si_data;
typedef struct message_si_close_connection_t
{
	uv_tcp_t *socket;
} message_si_close_connection;
typedef struct message_so_data_t
{
	uv_tcp_t *socket;
	uv_buf_t data;
} message_so_data;

typedef struct message_so_close_connection_t
{
	uv_tcp_t *socket;
} message_so_close_connection;
typedef struct message_gi_send_packet_t
{
	struct packet_t packet;
	struct client_t *client;
} message_gi_send_packet;

typedef struct message_gi_close_client_t
{
	struct client_t *client;
} message_gi_close_client;

typedef struct message_go_new_client_t
{
	uv_tcp_t *client;
} message_go_new_client;

typedef struct message_go_packet_t
{
	struct client_t *client;
	struct packet_t packet;
} message_go_packet;



