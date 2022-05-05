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


void on_write_end(uv_write_t* req, int status)
{
	if (status == -1) {
		fprintf(stderr, "error on_write_end");
		return;
	}

}
void server_send_packet(struct client_t *client, struct packet_t *packet)
{
	size_t i;
	size_t j;
	buffer_t send_buffer;
	size_t buf_size;
	uint8_t* data;

	uint8_t* packet_length_varint;
	size_t packet_length_varint_length;

	uint8_t* packet_id_varint;
	size_t packet_id_varint_length;

	struct slab_t current_slab;
	struct field_t current_field;

	i32 i32;
	u16 u16;
	u8 u8;
	i64* i64;
	char** str;
	struct uuid_t* uuid;
	bool boolean;
	double* ddouble;
	i16 i16;
	struct position_t* position;
	i8 i8;
	float ffloat;

	uv_buf_t socket_buffer[3];
	size_t send_buffer_length;
	assert(packet->direction == CLIENTBOUND);

	send_buffer = construct_buffer();


	for (i = 0; i < slabinfo.num_slabs; i++)
	{
		current_slab = slabinfo.slabs[i];
		if (!strcmp(packet->type, current_slab.name)) {
			/*this is the correct slab for the packet type*/
			for (j = 0; j < current_slab.num_fields; j++)
			{
				current_field = current_slab.fields[j];

				switch (current_field.fieldinfo.type)
				{
				case FT_VARINT:
					map_get(packet->map, current_field.field_name, &i32);
					data = make_varint(i32, &buf_size);
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_UNSIGNED_SHORT:
					map_get(packet->map, current_field.field_name, &u16);
					data = make_u16(u16, &buf_size);
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_UNSIGNED_BYTE:
					map_get(packet->map, current_field.field_name, &u8);
					data = make_u8(u8, &buf_size);
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_LONG:
					map_get(packet->map, current_field.field_name, &i64);
					data = make_i64(*i64, &buf_size);
					free(i64);
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_CHAT:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_UUID:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_BOOLEAN:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_DOUBLE:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_ANGLE:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_INT:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_SHORT:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_POSITION:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_BYTE:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_FLOAT:
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_STRING:
					map_get(packet->map, current_field.field_name, &str);
					data = make_string(str, &buf_size);
					buffer_append(send_buffer, data, buf_size);
					break;
				case FT_VARLONG:
					buffer_append(send_buffer, data, buf_size);
					break;
				default:
					break;
				}

			}

			break;
		}
	}

	data = buffer_get(send_buffer, &send_buffer_length);
	buffer_free(send_buffer);

	packet_id_varint = make_varint(current_slab.id, &packet_id_varint_length);
	packet_length_varint = make_varint(send_buffer_length + packet_id_varint_length, &packet_length_varint_length);

	socket_buffer[0].base = packet_length_varint;
	socket_buffer[0].len = packet_length_varint_length;

	socket_buffer[1].base = packet_id_varint;
	socket_buffer[1].len = packet_id_varint_length;

	socket_buffer[2].base = data;
	socket_buffer[2].len = send_buffer_length;

	uv_write_t* req1 = malloc(sizeof(uv_write_t));

	uv_write(req1, client->socket, socket_buffer, 3, on_write_end);

}



