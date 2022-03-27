#include "mcs.h"
#include "mc_packets.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#define CHECKRESULT do { \
	if(result) \
	{ \
		fprintf(stderr, "Error: %s\n", uv_strerror(result)); \
	} \
	} while(false)


uv_loop_t network_loop;

static uv_tcp_t server;
static struct sockaddr_in address;

static void on_new_connection(uv_stream_t *server, int status);
static void allocate_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
static void read_stream(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

void init_network_loop(void)
{
	int result;
	uv_loop_init(&network_loop);
	uv_tcp_init(&network_loop, &server);
	uv_ip4_addr("localhost", 25565, &address);
	result = uv_tcp_bind(&server, (const struct sockaddr *)&address, 0);
	CHECKRESULT;
	result = uv_listen((uv_stream_t *)&server, 128, on_new_connection);
	CHECKRESULT;
}

static void on_new_connection(uv_stream_t *server, int status)
{
	int result;
	uv_tcp_t *client = malloc(sizeof(uv_tcp_t));
	uv_tcp_init(&network_loop, client);
	result = uv_accept(server, (uv_stream_t *)client);
	CHECKRESULT;


	uv_read_start((uv_stream_t *)client, allocate_buffer, read_stream);
	printf("accepted client\n");
}

static void allocate_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	buf->base = malloc(suggested_size);
	buf->len = suggested_size;
}

static void handle_read(uv_stream_t *stream, uint32_t size, uint8_t *data)
{
	bool res;
	int32_t protocol_version;
	char *address;
	uint16_t port;
	int32_t next_state;
	struct client_t *client;
	struct wraparound_t wraparound;

	struct packet_metadata_t metadata = get_packet_metadata(size, data);

	if (should_wraparound(data, size, &wraparound))
	{
		handle_read(stream, wraparound.first_size, data);
		handle_read(stream, wraparound.second_size, data + wraparound.cutoff);
		return;
	}

	if (0 == (client = game_get_client(stream)))
	{
		client = game_init_client(stream);
	}


	struct packet_t packet;

	res = create_serverbound_packet(size, data, client->state, &packet);

	if (res)
	{
		game_handle_client_packet(client, &packet);
	}
}

static void read_stream(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	if (nread < 0)
	{
		if (nread == UV_EOF)
		{
			game_handle_client_disconnect(stream);
		}
	}
	else
	{
		handle_read(stream, nread, (uint8_t *)buf->base);
		if (buf->base)
		{
			free(buf->base);
		}
	}
}




void run_network_loop(void *data)
{
	uv_run(&network_loop, UV_RUN_DEFAULT);
}
