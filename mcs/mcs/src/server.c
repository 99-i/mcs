#include "mcs.h"
#include "mc_packets.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include "slab.h"
#include "slab_field.h"

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
}

static void allocate_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	buf->base = mcsalloc(suggested_size);
	buf->len = suggested_size;
}

static void handle_read(uv_stream_t *stream, buf data, bool is_wraparound)
{
	
	bool res;
	int i = 0;
	i++;

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
		return;
	}

	if (0 == (client = game_get_client((uv_tcp_t*) stream)))
	{
		client = game_init_client((uv_tcp_t*) stream);
	}


	struct packet_t packet;

	res = create_serverbound_packet(data, client->state, &packet);

	if (res)
	{
		game_handle_client_packet(client, &packet);
		map_destroy(packet.map);
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
		mcsfree(readbuf->base);
	}
}



void run_network_loop(void *data)
{
	uv_run(&network_loop, UV_RUN_DEFAULT);
}

struct write_context
{
	uv_write_t req;
	uv_buf_t* bufs;
};

void on_write_end(uv_write_t* req, int status)
{
	if (status == -1)
	{
		fprintf(stderr, "error on_write_end");
		return;
	}
	struct write_context* context = (struct write_context*) req;
	i32 i;
	for(i = 0; i < 3; i++)
	{
		free(context->bufs[i].base);
	}
	free(context->bufs);
	free(context);
}

void server_send_packet(struct client_t* client, struct packet_t* packet)
{
	struct write_context* context;
	uv_buf_t* bufs;
	buf length;
	buf id;
	buf data = buf_construct();

	buf temp;
	i32 i;
	struct slab_t* current_slab = 0;
	struct field_t* current_field = 0;

	for(i = 0; i < slabinfo.slabs.size; i++)
	{
		if(!strcmp(packet->type, slabinfo.slabs.fields[i].name))
		{
			current_slab = &slabinfo.slabs.fields[i];
		}
	}
	assert(current_slab != 0);


	for(i = 0; i < current_slab->fields.size; i++)
	{
		current_field = &current_slab->fields.fields[i];

		switch(current_field->type)
		{
			case FT_VARINT:
				temp = make_varint(map_get_str(packet->map, current_field->field_name).i32);
			break;
			case FT_UNSIGNED_SHORT:
				temp = make_u16(map_get_str(packet->map, current_field->field_name).u16);
			break;
			case FT_UNSIGNED_BYTE:
				temp = make_u8(map_get_str(packet->map, current_field->field_name).u8);
			break;
			case FT_LONG:
				temp = make_i64(map_get_str(packet->map, current_field->field_name).i64);
			break;
			case FT_UUID:
				temp = make_uuid(map_get_str(packet->map, current_field->field_name).uuid);
			break;
			case FT_BOOLEAN:
				temp = make_u8(map_get_str(packet->map, current_field->field_name).u8);
			break;
			case FT_DOUBLE:
				temp = make_double(map_get_str(packet->map, current_field->field_name).d);
			break;
			case FT_ANGLE:
				temp = make_angle(map_get_str(packet->map, current_field->field_name).u8);
			break;
			case FT_INT:
				temp = make_i32(map_get_str(packet->map, current_field->field_name).i32);
			break;
			case FT_SHORT:
				temp = make_i16(map_get_str(packet->map, current_field->field_name).i16);
			break;
			case FT_POSITION:
				temp = make_position(map_get_str(packet->map, current_field->field_name).pos);
			break;
			case FT_BYTE:
				temp = make_i8(map_get_str(packet->map, current_field->field_name).i8);
			break;
			case FT_FLOAT:
				temp = make_float(map_get_str(packet->map, current_field->field_name).f);
			break;
			case FT_STRING:
				temp = make_string(map_get_str(packet->map, current_field->field_name).str);
			break;
			case FT_VARLONG:
				temp = make_varlong(map_get_str(packet->map, current_field->field_name).i64);
			break;
			default:
				assert(false && "unreachable");
			break;
		}

		buf_append_buf(&data, temp);

		buf_destroy(&temp);

	}

	id = make_varint(current_slab->id);

	i = id.size + data.size;

	length = make_varint(i);

	bufs = mcsalloc(sizeof(uv_buf_t) * 3);
	bufs[0].len = length.size;
	bufs[1].len = id.size;
	bufs[2].len = data.size;


	bufs[0].base = mcsalloc(sizeof(char) * bufs[0].len);
	bufs[1].base = mcsalloc(sizeof(char) * bufs[1].len);
	bufs[2].base = mcsalloc(sizeof(char) * bufs[2].len);

	memcpy(bufs[0].base, length.data, length.size);

	memcpy(bufs[1].base, id.data, id.size);

	memcpy(bufs[2].base, data.data, data.size);

	context = mcsalloc(sizeof(struct write_context));

	context->bufs = bufs;

	uv_write(&context->req, (uv_stream_t*) client->socket, bufs, 3, on_write_end);

}


