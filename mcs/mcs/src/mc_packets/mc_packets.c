#include "mc_packets.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson.h"
#include <assert.h>
#include "mcs.h"
#undef uuid_t


struct slabinfo_t slabinfo;
static uint8_t read_varint(uint8_t *data, uint8_t max, int32_t *dest);
static uint8_t read_unsigned_short(uint8_t *data, uint8_t max, uint16_t *dest);
static uint8_t read_unsigned_byte(uint8_t *data, uint8_t max, uint8_t *dest);
static uint8_t read_long(uint8_t *data, uint8_t max, int64_t *dest);
static uint8_t read_unsigned_long(uint8_t *data, uint8_t max, uint64_t *dest);
static uint8_t read_chat(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_uuid(uint8_t *data, uint8_t max, struct uuid_t *dest);
static uint8_t read_boolean(uint8_t *data, uint8_t max, _Bool *dest);
static uint8_t read_double(uint8_t *data, uint8_t max, double *dest);
static uint8_t read_int(uint8_t *data, uint8_t max, int32_t *dest);
static uint8_t read_short(uint8_t *data, uint8_t max, int16_t *dest);
static uint8_t read_position(uint8_t *data, uint8_t max, struct position_t *dest);
static uint8_t read_byte(uint8_t *data, uint8_t max, int8_t *dest);
static uint8_t read_float(uint8_t *data, uint8_t max, float *dest);
static uint8_t read_string(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_varlong(uint8_t *data, uint8_t max, int64_t *dest);

struct packet_metadata_t get_packet_metadata(uint32_t data_length, uint8_t *data)
{
	uint8_t bytes = 0;

	int64_t id;
	int64_t length;

	bytes = read_varint(data, data_length, &length);
	bytes += read_varint(data + bytes, data_length - bytes, &id);

	return (struct packet_metadata_t)
	{
		(uint8_t)id, (uint16_t)length
	};
}

enum epacket_direction bound_to_to_direction(char *str);
enum estate state_str_to_state(char *str);
enum efield_type field_str_to_field_type(char *str);
void construct_slabs(void)
{
	char *jsonstr;
	cJSON *all;
	cJSON *current_slab;
	cJSON *fields;
	int num_slabs;
	int num_fields;
	uint32_t slab_index = 0;
	char *packet_direction;
	char *packet_state;
	size_t i;

	slurp_file_to_cstr("slabs.json", &jsonstr);
	all = cJSON_Parse(jsonstr);
	num_slabs = cJSON_GetArraySize(all);
	current_slab = all->child;
	slabinfo.num_slabs = num_slabs;
	slabinfo.slabs = calloc(num_slabs, sizeof(struct slab_t));

	do
	{
		slabinfo.slabs[slab_index].id = cJSON_GetObjectItem(current_slab, "id")->valueint;
		slabinfo.slabs[slab_index].name = _strdup(cJSON_GetObjectItem(current_slab, "name")->valuestring);

		packet_direction = cJSON_GetObjectItem(current_slab, "boundTo")->valuestring;
		packet_state = cJSON_GetObjectItem(current_slab, "state")->valuestring;

		slabinfo.slabs[slab_index].direction = bound_to_to_direction(packet_direction);
		slabinfo.slabs[slab_index].state = state_str_to_state(packet_state);
		fields = cJSON_GetObjectItem(current_slab, "fields");

		num_fields = cJSON_GetArraySize(fields);
		slabinfo.slabs[slab_index].num_fields = num_fields;
		slabinfo.slabs[slab_index].fields = calloc(num_fields, sizeof(struct field_t));

		for (i = 0; i < num_fields; i++)
		{
			slabinfo.slabs[slab_index].fields[i].field_name = _strdup(cJSON_GetObjectItem(cJSON_GetArrayItem(fields, i), "fieldName")->valuestring);
			slabinfo.slabs[slab_index].fields[i].fieldinfo.type = field_str_to_field_type(cJSON_GetObjectItem(cJSON_GetArrayItem(fields, i), "fieldType")->valuestring);
		}

		slab_index++;
	} while (current_slab = current_slab->next);

}

enum epacket_direction bound_to_to_direction(char *str)
{
	if (!strcmp(str, "Server"))
	{
		return SERVERBOUND;
	}
	else if (!strcmp(str, "Client"))
	{
		return CLIENTBOUND;
	}
}

enum estate state_str_to_state(char *str)
{
	if (!strcmp(str, "Handshaking"))
	{
		return STATE_HANDSHAKING;
	}
	else if (!strcmp(str, "Status"))
	{
		return STATE_STATUS;
	}
	else if (!strcmp(str, "Login"))
	{
		return STATE_LOGIN;
	}
	else if (!strcmp(str, "Play"))
	{
		return STATE_PLAY;
	}
}

enum efield_type field_str_to_field_type(char *str)
{
	if (!strcmp(str, "VarInt"))
	{
		return FT_VARINT;
	}
	else if (!strcmp(str, "Unsigned Short"))
	{
		return FT_UNSIGNED_SHORT;
	}
	else if (!strcmp(str, "Unsigned Byte"))
	{
		return FT_UNSIGNED_BYTE;
	}
	else if (!strcmp(str, "Long"))
	{
		return FT_LONG;
	}
	else if (!strcmp(str, "Chat"))
	{
		return FT_CHAT;
	}
	else if (!strcmp(str, "UUID"))
	{
		return FT_UUID;
	}
	else if (!strcmp(str, "Identifier"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "Boolean"))
	{
		return FT_BOOLEAN;
	}
	else if (!strcmp(str, "Double"))
	{
		return FT_DOUBLE;
	}
	else if (!strcmp(str, "Angle"))
	{
		return FT_ANGLE;
	}
	else if (!strcmp(str, "Int"))
	{
		return FT_INT;
	}
	else if (!strcmp(str, "Short"))
	{
		return FT_SHORT;
	}
	else if (!strcmp(str, "Position"))
	{
		return FT_POSITION;
	}
	else if (!strcmp(str, "Byte"))
	{
		return FT_BYTE;
	}
	else if (!strcmp(str, "Float"))
	{
		return FT_FLOAT;
	}
	else if (!strcmp(str, "String (16)"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "String (20)"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "String (40)"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "String (255)"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "String (256)"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "String (384)"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "String (32500)"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "String (32767)"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "String"))
	{
		return FT_STRING;
	}
	else if (!strcmp(str, "VarLong"))
	{
		return FT_VARLONG;
	}
	else if (!strcmp(str, "VarInt Enum"))
	{
		return FT_VARINT;
	}
	assert(false && "unreachable");
}

bool should_wraparound(uint8_t *data, uint32_t data_size, struct wraparound_t *cutoff)
{
	int32_t length;
	uint8_t length_size;
	length_size = read_varint(data, data_size, &length);

	if (data_size - length_size > length)
	{
		cutoff->cutoff = length_size + length;
		cutoff->first_size = length_size + length;
		cutoff->second_size = data_size - (length_size + length);
		return true;
	}

	return false;
}

bool create_serverbound_packet(uint32_t data_size, uint8_t *data, enum estate state, struct packet_t *packet)
{
	size_t i;
	size_t j;
	struct packet_metadata_t metadata = get_packet_metadata(data_size, data);
	bool found_slab = false;
	uint32_t data_needle = 0;
	data_needle += read_varint(data, data_size, 0);
	data_needle += read_varint(data + data_needle, data_size - data_needle, 0);

	int32_t the_varint;
	char** the_string;
	uint16_t the_unsigned_short;
	uint8_t the_unsigned_byte;
	int64_t* the_long;
	struct uuid_t* the_uuid;
	bool the_boolean;
	double* the_double;
	int32_t the_int;
	int16_t the_short;
	struct position_t* the_position;
	int8_t the_byte;
	float the_float;

	for (i = 0; i < slabinfo.num_slabs; i++)
	{
		if (slabinfo.slabs[i].state == state && slabinfo.slabs[i].id == metadata.packet_id && slabinfo.slabs[i].direction == SERVERBOUND)
		{
			found_slab = true;
			struct slab_t *slab = &slabinfo.slabs[i];
			packet->type = _strdup(slab->name);
			packet->map = construct_map();
			packet->direction = SERVERBOUND;

			for (j = 0; j < slab->num_fields; j++)
			{
				switch (slab->fields[j].fieldinfo.type)
				{
					case FT_VARINT:
						data_needle += read_varint(data + data_needle, data_size - data_needle, &the_varint);
						map_set(packet->map, slab->fields[j].field_name, the_varint);
						break;
					case FT_UNSIGNED_SHORT:
						data_needle += read_unsigned_short(data + data_needle, data_size - data_needle, &the_unsigned_short);
						map_set(packet->map, slab->fields[j].field_name, the_unsigned_short);
						break;
					case FT_UNSIGNED_BYTE:
						data_needle += read_unsigned_byte(data + data_needle, data_size - data_needle, &the_unsigned_byte);
						map_set(packet->map, slab->fields[j].field_name, the_unsigned_byte);
						break;
					case FT_LONG:
						the_long = malloc(sizeof(int64_t));
						data_needle += read_long(data + data_needle, data_size - data_needle, the_long);
						map_set(packet->map, slab->fields[j].field_name, the_long);
						break;
					case FT_CHAT:
						the_string = malloc(sizeof(char*));
						data_needle += read_chat(data + data_needle, data_size - data_needle, the_string);
						map_set(packet->map, slab->fields[j].field_name, *the_string);
						break;
					case FT_UUID:
						the_uuid = malloc(sizeof(struct uuid_t));
						data_needle += read_uuid(data + data_needle, data_size - data_needle, the_uuid);
						map_set(packet->map, slab->fields[j].field_name, the_uuid);
						break;
					case FT_BOOLEAN:
						data_needle += read_boolean(data + data_needle, data_size - data_needle, &the_boolean);
						map_set(packet->map, slab->fields[j].field_name, the_boolean);
						break;
					case FT_DOUBLE:
						the_double = malloc(sizeof(double));
						data_needle += read_double(data + data_needle, data_size - data_needle, the_double);
						map_set(packet->map, slab->fields[j].field_name, *((void**)&the_double));
						break;
					case FT_ANGLE:
						data_needle += read_unsigned_byte(data + data_needle, data_size - data_needle, &the_unsigned_byte);
						map_set(packet->map, slab->fields[j].field_name, the_unsigned_byte);
						break;
					case FT_INT:
						data_needle += read_int(data + data_needle, data_size - data_needle, &the_int);
						map_set(packet->map, slab->fields[j].field_name, the_int);
						break;
					case FT_SHORT:
						data_needle += read_short(data + data_needle, data_size - data_needle, &the_short);
						map_set(packet->map, slab->fields[j].field_name, the_short);
						break;
					case FT_POSITION:
						the_position = malloc(sizeof(struct position_t));
						data_needle += read_position(data + data_needle, data_size - data_needle, the_position);
						map_set(packet->map, slab->fields[j].field_name, the_position);
						break;
					case FT_BYTE:
						data_needle += read_byte(data + data_needle, data_size - data_needle, &the_byte);
						map_set(packet->map, slab->fields[j].field_name, the_byte);
						break;
					case FT_FLOAT:
						data_needle += read_float(data + data_needle, data_size - data_needle, &the_float);
						map_set(packet->map, slab->fields[j].field_name, *((void**)&the_float));
						break;
					case FT_STRING:
						the_string = malloc(sizeof(char*));
						data_needle += read_string(data + data_needle, data_size - data_needle, the_string);
						map_set(packet->map, slab->fields[j].field_name, *the_string);
						break;
					case FT_VARLONG:
						the_long = malloc(sizeof(int64_t));
						data_needle += read_varlong(data + data_needle, data_size - data_needle, the_long);
						map_set(packet->map, slab->fields[j].field_name, the_long);
						break;
					default:
						break;
				}
			}

			break;
		}
	}
	return found_slab;
}

static uint8_t read_varint(uint8_t *data, uint8_t max, int32_t *dest)
{
	int32_t decoded_int = 0;
	int32_t bit_offset = 0;
	uint8_t current_byte = 0;
	uint8_t len = 0;

	do
	{
		current_byte = data[len];

		decoded_int |= (current_byte & 0b01111111) << bit_offset;

		if (bit_offset == 35)
		{
			return 0;
			break;
		}
		bit_offset += 7;
		len++;
	} while ((current_byte & 0b10000000) != 0);

	if (dest)
	{
		*dest = decoded_int;
	}
	return len;
}
static uint8_t read_unsigned_short(uint8_t *data, uint8_t max, uint16_t *dest)
{
	if (max < 2)
	{
		return 0;
	}
	*dest = 0;
	*dest = (data[0] << 8) | data[1];
	return 2;
}
static uint8_t read_unsigned_byte(uint8_t *data, uint8_t max, uint8_t *dest)
{
	if (max < 1)
	{
		return 0;
	}
	*dest = data[0];
	return 1;
}
static uint8_t read_long(uint8_t *data, uint8_t max, int64_t *dest)
{
	if (max < 8)
	{
		return 0;
	}
	*dest = 0;
	*dest = (((uint64_t)data[0]) << 56) |
			(((uint64_t)data[1]) << 48) |
			(((uint64_t)data[2]) << 40) |
			(((uint64_t)data[3]) << 32) |
			(((uint64_t)data[4]) << 24) |
			(((uint64_t)data[5]) << 16) |
			(((uint64_t)data[6]) << 8) |
			(((uint64_t)data[7]) << 0);
	return 8;
}
static uint8_t read_chat(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 262144)
	{
		return 0;
	}
	str = malloc(strlength + 1);
	for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str);
			return 0;
		}
		str[i] = data[i + varint_length];
	}
	str[strlength] = 0; *dest = _strdup(str);
	return i + varint_length;
}
static uint8_t read_uuid(uint8_t *data, uint8_t max, struct uuid_t *dest)
{
	uint64_t high;
	uint64_t low;
	if (max < 16)
	{
		return 0;
	}
	high = read_unsigned_long(data, max, &high);
	low = read_unsigned_long(data + 8, max - 8, &low);

	dest->high = high;
	dest->low = low;
	return 16;
}
static uint8_t read_boolean(uint8_t *data, uint8_t max, _Bool *dest)
{
	if (max < 1)
	{
		return 0;
	}
	if (dest)
	{
		*dest = !!data[0];
	}
	return 1;
}
static uint8_t read_double(uint8_t *data, uint8_t max, double *dest)
{
	uint64_t holder;
	if (max < 8)
	{
		return 0;
	}
	read_unsigned_long(data, max, &holder);

	if (dest)
	{
		*dest = *(double *)&holder;
	}
	return 8;
}
static uint8_t read_int(uint8_t *data, uint8_t max, int32_t *dest)
{
	if (max < 4)
	{
		return 0;
	}
	if (dest)
	{
		*dest = 0;
		*dest = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	}
	return 4;
}
static uint8_t read_short(uint8_t *data, uint8_t max, int16_t *dest)
{
	if (max < 2)
	{
		return 0;
	}
	if (dest)
	{

		*dest = 0;
		*dest = (data[1] << 8) | data[0];
	}
	return 2;
}
static uint8_t read_position(uint8_t *data, uint8_t max, struct position_t *dest)
{
	uint64_t l;
	if (max < 8)
	{
		return 0;
	}
	read_unsigned_long(data, max, &l);
	if (dest)
	{

		dest->x = (l >> 38);
		dest->y = (l & 0xFF);
		dest->z = (l >> 12) & 0x3FFFFFF;
	}

	return 8;
}
static uint8_t read_unsigned_long(uint8_t *data, uint8_t max, uint64_t *dest)
{
	int64_t l;
	if (max < 8)
	{
		return 0;
	}
	read_long(data, max, &l);

	if (dest)
	{
		*dest = *((uint64_t *)&l);
	}
	return 8;
}
static uint8_t read_byte(uint8_t *data, uint8_t max, int8_t *dest)
{
	int8_t l;
	if (max < 1)
	{
		return 0;
	}
	read_unsigned_byte(data, max, &l);

	if (dest)
	{
		*dest = *((int8_t *)&l);
	}
	return 1;
}
static uint8_t read_float(uint8_t *data, uint8_t max, float *dest)
{

}
static uint8_t read_string(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 32767)
	{
		return 0;
	}
	str = malloc(strlength + 1);
	for (i = 0; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str);
			return 0;
		}
		str[i] = data[i + varint_length];
	}
	str[strlength] = 0; *dest = _strdup(str);
	return i + varint_length;
}

struct packet_t* construct_clientbound_packet(char* packet_type, ...)
{
	struct packet_t* packet = malloc(sizeof(struct packet_t));
	struct slab_t* current_slab = 0;
	struct field_t* current_field = 0;

	va_list argp;

	va_start(argp, packet_type);

	size_t i;

	packet->direction = CLIENTBOUND;

	for (i = 0; i < slabinfo.num_slabs; i++)
	{
		if (!strcmp(packet_type, slabinfo.slabs[i].name))
		{
			current_slab = &slabinfo.slabs[i];
		}
	}

	assert(current_slab != 0);
	assert(current_slab->direction == CLIENTBOUND);

	packet->type = packet_type;
	packet->map = construct_map();

	i32 int32;
	u16 uint16;
	u8 uint8;
	char* ch;

	for (i = 0; i < current_slab->num_fields; i++)
	{
		current_field = &current_slab->fields[i];

		switch (current_field->fieldinfo.type)
		{
		case FT_VARINT:
		case FT_INT:
			int32 = va_arg(argp, i32);
			break;
		case FT_UNSIGNED_SHORT:
			uint16 = va_arg(argp, u16);
			break;
		case FT_UNSIGNED_BYTE:
			uint8 = va_arg(argp, u8);
		case FT_ANGLE:
		case FT_BOOLEAN:
			break;
		case FT_LONG:
		case FT_VARLONG:
			break;
		case FT_UUID:
			break;
		case FT_DOUBLE:
			break;
		case FT_SHORT:
			break;
		case FT_POSITION:
			break;
		case FT_BYTE:
			break;
		case FT_FLOAT:
			break;
		case FT_STRING:
		case FT_CHAT:
			ch = va_arg(argp, char*);
			map_set(packet->map, current_field->field_name, _strdup(ch));
			break;
		default:
			break;
		}


	}

	va_end(argp);

	return packet;
}

void packet_free(struct packet_t* packet)
{

}


static uint8_t read_varlong(uint8_t *data, uint8_t max, int64_t *dest)
{
	int64_t decoded_long = 0;
	int32_t bit_offset = 0;
	int8_t current_byte = 0;
	uint8_t len = 0;

	do
	{
		current_byte = data[len];

		decoded_long |= (current_byte & 0b01111111) << bit_offset;

		if (bit_offset == 50)
		{
			return 0;
			break;
		}
		bit_offset += 7;
		len++;
	} while ((current_byte & 0b10000000) != 0);

	if (dest)
	{
		*dest = decoded_long;
	}
	return len;
}

uint8_t *make_varint(int32_t varint, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;
	size_t i = 0;
	while (true)
	{
		if (i == 5)
		{
			break;
		}
		if ((varint & ~0x7f) == 0)
		{
			buffer_append_byte(buf, varint);
			break;
		}

		buffer_append_byte(buf, (varint & 0x7f) | 0x80);
		varint >>= 7;
		i++;
	}

	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;


}
uint8_t *make_u16(u16 unsigned_short, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;
	buffer_append_byte(buf, (unsigned_short & 0xff00) >> 8);
	buffer_append_byte(buf, (unsigned_short & 0xff));

	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;
}
uint8_t *make_u8(u8 unsigned_byte, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;

	buffer_append_byte(buf, unsigned_byte);
	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;
}
uint8_t *make_i64(i64 l, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;

	buffer_append_byte(buf, (l & 0xFF00000000000000) >> 56);
	buffer_append_byte(buf, (l & 0xFF000000000000) >> 48);
	buffer_append_byte(buf, (l & 0xFF0000000000) >> 40);
	buffer_append_byte(buf, (l & 0xFF00000000) >> 32);
	buffer_append_byte(buf, (l & 0xFF000000) >> 24);
	buffer_append_byte(buf, (l & 0xFF0000) >> 16);
	buffer_append_byte(buf, (l & 0xFF00) >> 8);
	buffer_append_byte(buf, (l & 0xFF));
	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;
}
uint8_t *make_uuid(struct uuid_t uuid, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;

	buffer_append_byte(buf, (uuid.high & 0xFF00000000000000) >> 56);
	buffer_append_byte(buf, (uuid.high & 0xFF000000000000) >> 48);
	buffer_append_byte(buf, (uuid.high & 0xFF0000000000) >> 40);
	buffer_append_byte(buf, (uuid.high & 0xFF00000000) >> 32);
	buffer_append_byte(buf, (uuid.high & 0xFF000000) >> 24);
	buffer_append_byte(buf, (uuid.high & 0xFF0000) >> 16);
	buffer_append_byte(buf, (uuid.high & 0xFF00) >> 8);
	buffer_append_byte(buf, (uuid.high & 0xFF));
	buffer_append_byte(buf, (uuid.low & 0xFF00000000000000) >> 56);
	buffer_append_byte(buf, (uuid.low & 0xFF000000000000) >> 48);
	buffer_append_byte(buf, (uuid.low & 0xFF0000000000) >> 40);
	buffer_append_byte(buf, (uuid.low & 0xFF00000000) >> 32);
	buffer_append_byte(buf, (uuid.low & 0xFF000000) >> 24);
	buffer_append_byte(buf, (uuid.low & 0xFF0000) >> 16);
	buffer_append_byte(buf, (uuid.low & 0xFF00) >> 8);
	buffer_append_byte(buf, (uuid.low & 0xFF));

	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;
}
uint8_t *make_boolean(bool boolean, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;

	buffer_append_byte(buf, boolean);

	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;
}
uint8_t *make_double(double d, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;
	buffer_append_byte(buf, ((*(uint64_t *)&d) & 0xFF00000000000000) >> 56);
	buffer_append_byte(buf, ((*(uint64_t *)&d) & 0xFF000000000000) >> 48);
	buffer_append_byte(buf, ((*(uint64_t *)&d) & 0xFF0000000000) >> 40);
	buffer_append_byte(buf, ((*(uint64_t *)&d) & 0xFF00000000) >> 32);
	buffer_append_byte(buf, ((*(uint64_t *)&d) & 0xFF000000) >> 24);
	buffer_append_byte(buf, ((*(uint64_t *)&d) & 0xFF0000) >> 16);
	buffer_append_byte(buf, ((*(uint64_t *)&d) & 0xFF00) >> 8);
	buffer_append_byte(buf, ((*(uint64_t *)&d) & 0xFF));
	data = buffer_get(buf, length);
	buffer_free(buf);
	return data;
}
uint8_t *make_angle(uint8_t angle, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;

	buffer_append_byte(buf, angle);
	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;
}
uint8_t *make_i32(i32 integer, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;

	buffer_append_byte(buf, (integer & 0xff000000) >> 24);
	buffer_append_byte(buf, (integer & 0xff0000) >> 16);
	buffer_append_byte(buf, (integer & 0xff00) >> 8);
	buffer_append_byte(buf, (integer & 0xff));

	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;
}
uint8_t *make_i16(i16 s, size_t *length)
{
	buffer_t buf = construct_buffer();
	u8 *data;

	buffer_append_byte(buf, (s & 0xff00) >> 8);
	buffer_append_byte(buf, (s & 0xff));

	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;

}
uint8_t *make_position(struct position_t pos, size_t *length)
{
	buffer_t buf = construct_buffer();
	u8 *data;
	i64 int_pos = 0;

	pos.x &= 0x3ffffff;
	pos.z &= 0x3ffffff;
	pos.y &= 0xfff;
	int_pos |= (int64_t)pos.x << (64 - 26);
	int_pos |= (int64_t)pos.z << (64 - 26 - 26);
	int_pos |= (int64_t)pos.y;


	return make_i64(int_pos, length);
}
uint8_t *make_i8(i8 byte, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;

	buffer_append_byte(buf, (byte & 0xff));

	data = buffer_get(buf, length);
	buffer_free(buf);
	
	return data;

}
uint8_t *make_float(float f, size_t *length)
{
	uint64_t data;
	data = *((uint64_t*)&f);

	return make_i64(data, length);
}
uint8_t *make_string(const char *str, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t* data;
	size_t str_len = strlen(str);
	size_t length_size;
	size_t i;
	uint8_t* str_length_varint = make_varint(str_len, &length_size);

	buffer_append(buf, str_length_varint, length_size);

	for (i = 0; i < str_len; i++)
	{
		buffer_append_byte(buf, str[i]);
	}

	data = buffer_get(buf, length);
	buffer_free(buf);

	return data;

}
uint8_t *make_varlong(int64_t varlong, size_t *length)
{
	buffer_t buf = construct_buffer();
	uint8_t *data;
	size_t i = 0;
	while (true)
	{
		if (i == 10)
		{
			break;
		}
		if ((varlong & ~0x7f) == 0)
		{
			buffer_append_byte(buf, varlong);
			break;
		}

		buffer_append_byte(buf, (varlong & 0x7f) | 0x80);
		varlong >>= 7;
		i++;
	}

	data = buffer_get(buf, length);
	buffer_free(buf);
	return data;
}
