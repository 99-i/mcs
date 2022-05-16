#include "mcs.h"
#include "mc_packets.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#pragma warning( disable: 4703)
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
	uv_tcp_t *client = mcsalloc(sizeof(uv_tcp_t));
	uv_tcp_init(&network_loop, client);
	result = uv_accept(server, (uv_stream_t *)client);
	CHECKRESULT;


	uv_read_start((uv_stream_t *)client, allocate_buffer, read_stream);
	printf("accepted client\n");
}

static void allocate_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	buf->base = mcsalloc(suggested_size);
	buf->len = suggested_size;
}

static void handle_read(uv_stream_t *stream, buf data, bool is_wraparound)
{
	//bool res;
	struct client_t *client;
	struct wraparound_t wraparound;
	buf sliced;
	struct packet_metadata_t metadata;
	
	metadata = get_packet_metadata(data);

	if (should_wraparound(data, &wraparound))
	{
		sliced.size = wraparound.first_size;
		sliced.data = data.data;
		handle_read(stream, sliced, true);
		sliced.size = wraparound.second_size;
		sliced.data = data.data + wraparound.cutoff;
		handle_read(stream, sliced, true);
		buf_destroy(&data);
		return;
	}

	if (0 == (client = game_get_client((uv_tcp_t*) stream)))
	{
		client = game_init_client((uv_tcp_t*) stream);
	}


	//struct packet_t packet;

	/*res = create_serverbound_packet(data, client->state, &packet);

	if (res)
	{
		game_handle_client_packet(client, &packet);
	}
	*/
	if (!is_wraparound)
	{
		buf_destroy(&data);
	}
}

static void read_stream(uv_stream_t *stream, ssize_t nread, const uv_buf_t *readbuf)
{
	buf handlebuf;
	if (nread < 0)
	{
		if (nread == UV_EOF)
		{
			game_handle_client_disconnect((uv_tcp_t*) stream);
			return;
		}
	}

	handlebuf.size = nread;
	handlebuf.data = readbuf->base;
	handle_read(stream, handlebuf, false);
	if (readbuf->base)
	{
		free(readbuf->base);
	}
}




void run_network_loop(void *data)
{
	uv_run(&network_loop, UV_RUN_DEFAULT);
}


void on_write_end(uv_write_t* req, int status)
{
	if (status == -1)
	{
		fprintf(stderr, "error on_write_end");
		return;
	}

}

void server_send_packet(struct client_t* client, struct packet_t* packet)
{
	//TODO
	assert(false && "TODO");
}


