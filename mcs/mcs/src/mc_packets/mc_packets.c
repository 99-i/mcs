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

struct slabinfo_t
{
	size_t num_slabs;
	struct slab_t *slabs;
}
slabinfo;

static uint8_t read_varint(uint8_t *data, uint8_t max, int32_t *dest);
static uint8_t read_string_255(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_unsigned_short(uint8_t *data, uint8_t max, uint16_t *dest);
static uint8_t read_unsigned_byte(uint8_t *data, uint8_t max, uint8_t *dest);
static uint8_t read_string_32767(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_long(uint8_t *data, uint8_t max, int64_t *dest);
static uint8_t read_unsigned_long(uint8_t *data, uint8_t max, uint64_t *dest);
static uint8_t read_chat(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_string_20(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_uuid(uint8_t *data, uint8_t max, struct uuid_t *dest);
static uint8_t read_string_16(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_identifier(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_boolean(uint8_t *data, uint8_t max, _Bool *dest);
static uint8_t read_double(uint8_t *data, uint8_t max, double *dest);
static uint8_t read_int(uint8_t *data, uint8_t max, int32_t *dest);
static uint8_t read_short(uint8_t *data, uint8_t max, int16_t *dest);
static uint8_t read_position(uint8_t *data, uint8_t max, struct position_t *dest);
static uint8_t read_byte(uint8_t *data, uint8_t max, int8_t *dest);
static uint8_t read_float(uint8_t *data, uint8_t max, float *dest);
static uint8_t read_string_40(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_string_256(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_string_32500(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_string(uint8_t *data, uint8_t max, char **dest);
static uint8_t read_varlong(uint8_t *data, uint8_t max, int64_t *dest);
static uint8_t read_string_384(uint8_t *data, uint8_t max, char **dest);

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
		return FT_IDENTIFIER;
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
		return FT_STRING_16;
	}
	else if (!strcmp(str, "String (20)"))
	{
		return FT_STRING_20;
	}
	else if (!strcmp(str, "String (40)"))
	{
		return FT_STRING_40;
	}
	else if (!strcmp(str, "String (255)"))
	{
		return FT_STRING_255;
	}
	else if (!strcmp(str, "String (256)"))
	{
		return FT_STRING_256;
	}
	else if (!strcmp(str, "String (384)"))
	{
		return FT_STRING_384;
	}
	else if (!strcmp(str, "String (32500)"))
	{
		return FT_STRING_32500;
	}
	else if (!strcmp(str, "String (32767)"))
	{
		return FT_STRING_32767;
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
	char *the_string_255 = 0;
	uint16_t the_unsigned_short;
	uint8_t the_unsigned_byte;
	char *the_string_32767 = 0;
	int64_t the_long;
	char *the_identifier = 0;
	char *the_chat = 0;
	char *the_string_20 = 0;
	struct uuid_t the_uuid;
	char *the_string_16 = 0;
	bool the_boolean;
	double the_double;
	int32_t the_int;
	int16_t the_short;
	struct position_t the_position;
	int8_t the_byte;
	float the_float;
	char *the_string_40 = 0;
	char *the_string_256 = 0;
	char *the_string_32500 = 0;
	char *the_string = 0;
	int64_t the_varlong;
	char *the_string_384 = 0;

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
						map_set_int32(packet->map, slab->fields[j].field_name, the_varint);
						break;
					case FT_UNSIGNED_SHORT:
						data_needle += read_unsigned_short(data + data_needle, data_size - data_needle, &the_unsigned_short);
						map_set_uint16(packet->map, slab->fields[j].field_name, the_unsigned_short);
						break;
					case FT_UNSIGNED_BYTE:
						data_needle += read_unsigned_byte(data + data_needle, data_size - data_needle, &the_unsigned_byte);
						map_set_uint8(packet->map, slab->fields[j].field_name, the_unsigned_byte);
						break;
					case FT_LONG:
						data_needle += read_long(data + data_needle, data_size - data_needle, &the_long);
						map_set_int64(packet->map, slab->fields[j].field_name, the_long);
						break;
					case FT_CHAT:
						data_needle += read_chat(data + data_needle, data_size - data_needle, &the_chat);
						map_set_string(packet->map, slab->fields[j].field_name, the_chat);
						break;
					case FT_UUID:
						data_needle += read_uuid(data + data_needle, data_size - data_needle, &the_uuid);
						map_set_uuid(packet->map, slab->fields[j].field_name, the_uuid);
						break;
					case FT_IDENTIFIER:
						data_needle += read_identifier(data + data_needle, data_size - data_needle, &the_identifier);
						map_set_string(packet->map, slab->fields[j].field_name, the_identifier);
						break;
					case FT_BOOLEAN:
						data_needle += read_boolean(data + data_needle, data_size - data_needle, &the_boolean);
						map_set_boolean(packet->map, slab->fields[j].field_name, the_boolean);
						break;
					case FT_DOUBLE:
						data_needle += read_double(data + data_needle, data_size - data_needle, &the_double);
						map_set_double(packet->map, slab->fields[j].field_name, the_double);
						break;
					case FT_ANGLE:
						data_needle += read_unsigned_byte(data + data_needle, data_size - data_needle, &the_unsigned_byte);
						map_set_uint8(packet->map, slab->fields[j].field_name, the_unsigned_byte);
						break;
					case FT_INT:
						data_needle += read_int(data + data_needle, data_size - data_needle, &the_int);
						map_set_int32(packet->map, slab->fields[j].field_name, the_int);
						break;
					case FT_SHORT:
						data_needle += read_short(data + data_needle, data_size - data_needle, &the_short);
						map_set_int16(packet->map, slab->fields[j].field_name, the_short);
						break;
					case FT_POSITION:
						data_needle += read_position(data + data_needle, data_size - data_needle, &the_position);
						map_set_position(packet->map, slab->fields[j].field_name, the_position);
						break;
					case FT_BYTE:
						data_needle += read_byte(data + data_needle, data_size - data_needle, &the_byte);
						map_set_uint8(packet->map, slab->fields[j].field_name, the_byte);
						break;
					case FT_FLOAT:
						data_needle += read_float(data + data_needle, data_size - data_needle, &the_float);
						map_set_float(packet->map, slab->fields[j].field_name, the_float);
						break;
					case FT_STRING_16:
						data_needle += read_string_16(data + data_needle, data_size - data_needle, &the_string_16);
						map_set_string(packet->map, slab->fields[j].field_name, the_string_16);
						break;
					case FT_STRING_20:
						data_needle += read_string_20(data + data_needle, data_size - data_needle, &the_string_20);
						map_set_string(packet->map, slab->fields[j].field_name, the_string_20);
						break;
					case FT_STRING_40:
						data_needle += read_string_40(data + data_needle, data_size - data_needle, &the_string_40);
						map_set_string(packet->map, slab->fields[j].field_name, the_string_40);
						break;
					case FT_STRING_255:
						data_needle += read_string_255(data + data_needle, data_size - data_needle, &the_string_255);
						map_set_string(packet->map, slab->fields[j].field_name, the_string_255);
						break;
					case FT_STRING_256:
						data_needle += read_string_256(data + data_needle, data_size - data_needle, &the_string_256);
						map_set_string(packet->map, slab->fields[j].field_name, the_string_256);
						break;
					case FT_STRING_384:
						data_needle += read_string_384(data + data_needle, data_size - data_needle, &the_string_384);
						map_set_string(packet->map, slab->fields[j].field_name, the_string_384);
						break;
					case FT_STRING_32500:
						data_needle += read_string_32500(data + data_needle, data_size - data_needle, &the_string_32500);
						map_set_string(packet->map, slab->fields[j].field_name, the_string_32500);
						break;
					case FT_STRING_32767:
						data_needle += read_string_32767(data + data_needle, data_size - data_needle, &the_string_32767);
						map_set_string(packet->map, slab->fields[j].field_name, the_string_32767);
						break;
					case FT_STRING:
						data_needle += read_string(data + data_needle, data_size - data_needle, &the_string);
						map_set_string(packet->map, slab->fields[j].field_name, the_string);
						break;
					case FT_VARLONG:
						data_needle += read_varlong(data + data_needle, data_size - data_needle, &the_varlong);
						map_set_int64(packet->map, slab->fields[j].field_name, the_varlong);
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
	*dest = ((uint64_t)data[0] << 40) | ((uint64_t)data[1] << 32) | ((uint64_t)data[2] << 24) | ((uint64_t)data[3] << 16) | ((uint64_t)data[4] << 8) | data[5];
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
static uint8_t read_string_40(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 40)
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
static uint8_t read_string_256(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 256)
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
static uint8_t read_string_32500(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 32500)
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
static uint8_t read_string(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 32767)
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
static uint8_t read_string_384(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 384)
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
static uint8_t read_identifier(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 32767)
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
static uint8_t read_string_16(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 16)
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
static uint8_t read_string_20(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 20)
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
	str[strlength] = 0;
	*dest = _strdup(str);
	return i + varint_length;
}
static uint8_t read_string_32767(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i = 0;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 32767)
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
static uint8_t read_string_255(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength;
	size_t i;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(data, max, &strlength);
	if (strlength > 255)
	{
		return 0;
	}
	if (strlength < 1)
	{
		return varint_length;

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
	str[strlength] = 0;
	*dest = _strdup(str);
	return i + varint_length;
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


bool write_packet_response(struct packet_t *packet, char *json_response)
{
	packet->type = _strdup("Response");
	packet->direction = CLIENTBOUND;
}
bool write_packet_pong(struct packet_t *packet, int64_t payload)
{
	packet->type = _strdup("Pong");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int64(packet->map, "Payload", payload);
}
bool write_packet_disconnect_login(struct packet_t *packet, char *reason)
{
	packet->type = _strdup("Disconnect (login)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_string(packet->map, "Reason", reason);
}
bool write_packet_login_success(struct packet_t *packet, struct uuid_t uuid, char *username)
{
	packet->type = _strdup("Login Success");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_uuid(packet->map, "UUID", uuid);
	map_set_string(packet->map, "Username", username);
}
bool write_packet_set_compression(struct packet_t *packet, int32_t threshold)
{
	packet->type = _strdup("Set Compression");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Threshold", threshold);
}
bool write_packet_spawn_entity(struct packet_t *packet, int32_t entity_id, struct uuid_t object_uuid, int32_t type, double x, double y, double z, uint8_t pitch, uint8_t yaw, int32_t data, int16_t velocity_x, int16_t velocity_y, int16_t velocity_z)
{
	packet->type = _strdup("Spawn Entity");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_uuid(packet->map, "Object UUID", object_uuid);
	map_set_int32(packet->map, "Type", type);
	map_set_double(packet->map, "X", x);
	map_set_double(packet->map, "Y", y);
	map_set_double(packet->map, "Z", z);
	map_set_uint8(packet->map, "Pitch", pitch);
	map_set_uint8(packet->map, "Yaw", yaw);
	map_set_int32(packet->map, "Data", data);
	map_set_int16(packet->map, "Velocity X", velocity_x);
	map_set_int16(packet->map, "Velocity Y", velocity_y);
	map_set_int16(packet->map, "Velocity Z", velocity_z);
}
bool write_packet_spawn_experience_orb(struct packet_t *packet, int32_t entity_id, double x, double y, double z, int16_t count)
{
	packet->type = _strdup("Spawn Experience Orb");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_double(packet->map, "X", x);
	map_set_double(packet->map, "Y", y);
	map_set_double(packet->map, "Z", z);
	map_set_int16(packet->map, "Count", count);
}
bool write_packet_spawn_living_entity(struct packet_t *packet, int32_t entity_id, struct uuid_t entity_uuid, int32_t type, double x, double y, double z, uint8_t yaw, uint8_t pitch, uint8_t head_pitch, int16_t velocity_x, int16_t velocity_y, int16_t velocity_z)
{
	packet->type = _strdup("Spawn Living Entity");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_uuid(packet->map, "Entity UUID", entity_uuid);
	map_set_int32(packet->map, "Type", type);
	map_set_double(packet->map, "X", x);
	map_set_double(packet->map, "Y", y);
	map_set_double(packet->map, "Z", z);
	map_set_uint8(packet->map, "Yaw", yaw);
	map_set_uint8(packet->map, "Pitch", pitch);
	map_set_uint8(packet->map, "Head Pitch", head_pitch);
	map_set_int16(packet->map, "Velocity X", velocity_x);
	map_set_int16(packet->map, "Velocity Y", velocity_y);
	map_set_int16(packet->map, "Velocity Z", velocity_z);
}
bool write_packet_spawn_player(struct packet_t *packet, int32_t entity_id, struct uuid_t player_uuid, double x, double y, double z, uint8_t yaw, uint8_t pitch)
{
	packet->type = _strdup("Spawn Player");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_uuid(packet->map, "Player UUID", player_uuid);
	map_set_double(packet->map, "X", x);
	map_set_double(packet->map, "Y", y);
	map_set_double(packet->map, "Z", z);
	map_set_uint8(packet->map, "Yaw", yaw);
	map_set_uint8(packet->map, "Pitch", pitch);
}
bool write_packet_entity_animation(struct packet_t *packet, int32_t entity_id, uint8_t animation)
{
	packet->type = _strdup("Entity Animation (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_uint8(packet->map, "Animation", animation);
}
bool write_packet_acknowledge_player_digging(struct packet_t *packet, struct position_t location, int32_t block, int32_t status, bool successful)
{
	packet->type = _strdup("Acknowledge Player Digging");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_position(packet->map, "Location", location);
	map_set_int32(packet->map, "Block", block);
	map_set_int32(packet->map, "Status", status);
	map_set_boolean(packet->map, "Successful", successful);
}
bool write_packet_block_break_animation(struct packet_t *packet, int32_t entity_id, struct position_t location, int8_t destroy_stage)
{
	packet->type = _strdup("Block Break Animation");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_position(packet->map, "Location", location);
	map_set_int8(packet->map, "Destroy Stage", destroy_stage);
}
bool write_packet_block_action(struct packet_t *packet, struct position_t location, uint8_t action_id_byte_1, uint8_t action_param_byte_2, int32_t block_type)
{
	packet->type = _strdup("Block Action");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_position(packet->map, "Location", location);
	map_set_uint8(packet->map, "Action ID (Byte 1)", action_id_byte_1);
	map_set_uint8(packet->map, "Action Param (Byte 2)", action_param_byte_2);
	map_set_int32(packet->map, "Block Type", block_type);
}
bool write_packet_block_change(struct packet_t *packet, struct position_t location, int32_t block_id)
{
	packet->type = _strdup("Block Change");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_position(packet->map, "Location", location);
	map_set_int32(packet->map, "Block ID", block_id);
}
bool write_packet_server_difficulty(struct packet_t *packet, uint8_t difficulty, bool difficulty_locked)
{
	packet->type = _strdup("Server Difficulty");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_uint8(packet->map, "Difficulty", difficulty);
	map_set_boolean(packet->map, "Difficulty locked?", difficulty_locked);
}
bool write_packet_chat_message(struct packet_t *packet, char *json_data, int8_t position, struct uuid_t sender)
{
	packet->type = _strdup("Chat Message (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_string(packet->map, "JSON Data", json_data);
	map_set_int8(packet->map, "Position", position);
	map_set_uuid(packet->map, "Sender", sender);
}
bool write_packet_window_confirmation(struct packet_t *packet, int8_t window_id, int16_t action_number, bool accepted)
{
	packet->type = _strdup("Window Confirmation (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int8(packet->map, "Window ID", window_id);
	map_set_int16(packet->map, "Action Number", action_number);
	map_set_boolean(packet->map, "Accepted", accepted);
}
bool write_packet_close_window(struct packet_t *packet, uint8_t window_id)
{
	packet->type = _strdup("Close Window (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_uint8(packet->map, "Window ID", window_id);
}
bool write_packet_window_property(struct packet_t *packet, uint8_t window_id, int16_t property, int16_t value)
{
	packet->type = _strdup("Window Property");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_uint8(packet->map, "Window ID", window_id);
	map_set_int16(packet->map, "Property", property);
	map_set_int16(packet->map, "Value", value);
}
bool write_packet_set_cooldown(struct packet_t *packet, int32_t item_id, int32_t cooldown_ticks)
{
	packet->type = _strdup("Set Cooldown");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Item ID", item_id);
	map_set_int32(packet->map, "Cooldown Ticks", cooldown_ticks);
}
bool write_packet_named_sound_effect(struct packet_t *packet, char *sound_name, int32_t sound_category, int32_t effect_position_x, int32_t effect_position_y, int32_t effect_position_z, float volume, float pitch)
{
	packet->type = _strdup("Named Sound Effect");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_string(packet->map, "Sound Name", sound_name);
	map_set_int32(packet->map, "Sound Category", sound_category);
	map_set_int32(packet->map, "Effect Position X", effect_position_x);
	map_set_int32(packet->map, "Effect Position Y", effect_position_y);
	map_set_int32(packet->map, "Effect Position Z", effect_position_z);
	map_set_float(packet->map, "Volume", volume);
	map_set_float(packet->map, "Pitch", pitch);
}
bool write_packet_disconnect_play(struct packet_t *packet, char *reason)
{
	packet->type = _strdup("Disconnect (play)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_string(packet->map, "Reason", reason);
}
bool write_packet_unload_chunk(struct packet_t *packet, int32_t chunk_x, int32_t chunk_z)
{
	packet->type = _strdup("Unload Chunk");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Chunk X", chunk_x);
	map_set_int32(packet->map, "Chunk Z", chunk_z);
}
bool write_packet_change_game_state(struct packet_t *packet, uint8_t reason, float value)
{
	packet->type = _strdup("Change Game State");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_uint8(packet->map, "Reason", reason);
	map_set_float(packet->map, "Value", value);
}
bool write_packet_open_horse_window(struct packet_t *packet, int8_t window_id, int32_t number_of_slots, int32_t entity_id)
{
	packet->type = _strdup("Open Horse Window");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int8(packet->map, "Window ID?", window_id);
	map_set_int32(packet->map, "Number of slots?", number_of_slots);
	map_set_int32(packet->map, "Entity ID?", entity_id);
}
bool write_packet_keep_alive(struct packet_t *packet, int64_t keep_alive_id)
{
	packet->type = _strdup("Keep Alive (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int64(packet->map, "Keep Alive ID", keep_alive_id);
}
bool write_packet_effect(struct packet_t *packet, int32_t effect_id, struct position_t location, int32_t data, bool disable_relative_volume)
{
	packet->type = _strdup("Effect");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Effect ID", effect_id);
	map_set_position(packet->map, "Location", location);
	map_set_int32(packet->map, "Data", data);
	map_set_boolean(packet->map, "Disable Relative Volume", disable_relative_volume);
}
bool write_packet_entity_position(struct packet_t *packet, int32_t entity_id, int16_t delta_x, int16_t delta_y, int16_t delta_z, bool on_ground)
{
	packet->type = _strdup("Entity Position");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_int16(packet->map, "Delta X", delta_x);
	map_set_int16(packet->map, "Delta Y", delta_y);
	map_set_int16(packet->map, "Delta Z", delta_z);
	map_set_boolean(packet->map, "On Ground", on_ground);
}
bool write_packet_entity_position_and_rotation(struct packet_t *packet, int32_t entity_id, int16_t delta_x, int16_t delta_y, int16_t delta_z, uint8_t yaw, uint8_t pitch, bool on_ground)
{
	packet->type = _strdup("Entity Position and Rotation");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_int16(packet->map, "Delta X", delta_x);
	map_set_int16(packet->map, "Delta Y", delta_y);
	map_set_int16(packet->map, "Delta Z", delta_z);
	map_set_uint8(packet->map, "Yaw", yaw);
	map_set_uint8(packet->map, "Pitch", pitch);
	map_set_boolean(packet->map, "On Ground", on_ground);
}
bool write_packet_entity_rotation(struct packet_t *packet, int32_t entity_id, uint8_t yaw, uint8_t pitch, bool on_ground)
{
	packet->type = _strdup("Entity Rotation");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_uint8(packet->map, "Yaw", yaw);
	map_set_uint8(packet->map, "Pitch", pitch);
	map_set_boolean(packet->map, "On Ground", on_ground);
}
bool write_packet_entity_movement(struct packet_t *packet, int32_t entity_id)
{
	packet->type = _strdup("Entity Movement");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
}
bool write_packet_vehicle_move(struct packet_t *packet, double x, double y, double z, float yaw, float pitch)
{
	packet->type = _strdup("Vehicle Move (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_double(packet->map, "X", x);
	map_set_double(packet->map, "Y", y);
	map_set_double(packet->map, "Z", z);
	map_set_float(packet->map, "Yaw", yaw);
	map_set_float(packet->map, "Pitch", pitch);
}
bool write_packet_open_book(struct packet_t *packet, int32_t hand)
{
	packet->type = _strdup("Open Book");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Hand", hand);
}
bool write_packet_open_window(struct packet_t *packet, int32_t window_id, int32_t window_type, char *window_title)
{
	packet->type = _strdup("Open Window");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Window ID", window_id);
	map_set_int32(packet->map, "Window Type", window_type);
	map_set_string(packet->map, "Window Title", window_title);
}
bool write_packet_open_sign_editor(struct packet_t *packet, struct position_t location)
{
	packet->type = _strdup("Open Sign Editor");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_position(packet->map, "Location", location);
}
bool write_packet_craft_recipe_response(struct packet_t *packet, int8_t window_id, char *recipe)
{
	packet->type = _strdup("Craft Recipe Response");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int8(packet->map, "Window ID", window_id);
	map_set_string(packet->map, "Recipe", recipe);
}
bool write_packet_player_abilities(struct packet_t *packet, int8_t flags, float flying_speed, float field_of_view_modifier)
{
	packet->type = _strdup("Player Abilities (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int8(packet->map, "Flags", flags);
	map_set_float(packet->map, "Flying Speed", flying_speed);
	map_set_float(packet->map, "Field of View Modifier", field_of_view_modifier);
}
bool write_packet_player_position_and_look(struct packet_t *packet, double x, double y, double z, float yaw, float pitch, int8_t flags, int32_t teleport_id)
{
	packet->type = _strdup("Player Position And Look (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_double(packet->map, "X", x);
	map_set_double(packet->map, "Y", y);
	map_set_double(packet->map, "Z", z);
	map_set_float(packet->map, "Yaw", yaw);
	map_set_float(packet->map, "Pitch", pitch);
	map_set_int8(packet->map, "Flags", flags);
	map_set_int32(packet->map, "Teleport ID", teleport_id);
}
bool write_packet_remove_entity_effect(struct packet_t *packet, int32_t entity_id, int8_t effect_id)
{
	packet->type = _strdup("Remove Entity Effect");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_int8(packet->map, "Effect ID", effect_id);
}
bool write_packet_resource_pack_send(struct packet_t *packet, char *url, char *hash)
{
	packet->type = _strdup("Resource Pack Send");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_string(packet->map, "URL", url);
	map_set_string(packet->map, "Hash", hash);
}
bool write_packet_entity_head_look(struct packet_t *packet, int32_t entity_id, uint8_t head_yaw)
{
	packet->type = _strdup("Entity Head Look");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_uint8(packet->map, "Head Yaw", head_yaw);
}
bool write_packet_select_advancement_tab(struct packet_t *packet, bool has_id, char *optional_identifier)
{
	packet->type = _strdup("Select Advancement Tab");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_boolean(packet->map, "Has id", has_id);
	map_set_string(packet->map, "Optional Identifier", optional_identifier);
}
bool write_packet_camera(struct packet_t *packet, int32_t camera_id)
{
	packet->type = _strdup("Camera");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Camera ID", camera_id);
}
bool write_packet_held_item_change(struct packet_t *packet, int8_t slot)
{
	packet->type = _strdup("Held Item Change (clientbound)");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int8(packet->map, "Slot", slot);
}
bool write_packet_update_view_position(struct packet_t *packet, int32_t chunk_x, int32_t chunk_z)
{
	packet->type = _strdup("Update View Position");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Chunk X", chunk_x);
	map_set_int32(packet->map, "Chunk Z", chunk_z);
}
bool write_packet_update_view_distance(struct packet_t *packet, int32_t view_distance)
{
	packet->type = _strdup("Update View Distance");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "View Distance", view_distance);
}
bool write_packet_spawn_position(struct packet_t *packet, struct position_t location)
{
	packet->type = _strdup("Spawn Position");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_position(packet->map, "Location", location);
}
bool write_packet_display_scoreboard(struct packet_t *packet, int8_t position, char *score_name)
{
	packet->type = _strdup("Display Scoreboard");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int8(packet->map, "Position", position);
	map_set_string(packet->map, "Score Name", score_name);
}
bool write_packet_attach_entity(struct packet_t *packet, int32_t attached_entity_id, int32_t holding_entity_id)
{
	packet->type = _strdup("Attach Entity");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Attached Entity ID", attached_entity_id);
	map_set_int32(packet->map, "Holding Entity ID", holding_entity_id);
}
bool write_packet_entity_velocity(struct packet_t *packet, int32_t entity_id, int16_t velocity_x, int16_t velocity_y, int16_t velocity_z)
{
	packet->type = _strdup("Entity Velocity");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_int16(packet->map, "Velocity X", velocity_x);
	map_set_int16(packet->map, "Velocity Y", velocity_y);
	map_set_int16(packet->map, "Velocity Z", velocity_z);
}
bool write_packet_set_experience(struct packet_t *packet, float experience_bar, int32_t level, int32_t total_experience)
{
	packet->type = _strdup("Set Experience");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_float(packet->map, "Experience bar", experience_bar);
	map_set_int32(packet->map, "Level", level);
	map_set_int32(packet->map, "Total Experience", total_experience);
}
bool write_packet_update_health(struct packet_t *packet, float health, int32_t food, float food_saturation)
{
	packet->type = _strdup("Update Health");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_float(packet->map, "Health", health);
	map_set_int32(packet->map, "Food", food);
	map_set_float(packet->map, "Food Saturation", food_saturation);
}
bool write_packet_time_update(struct packet_t *packet, int64_t world_age, int64_t time_of_day)
{
	packet->type = _strdup("Time Update");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int64(packet->map, "World Age", world_age);
	map_set_int64(packet->map, "Time of day", time_of_day);
}
bool write_packet_entity_sound_effect(struct packet_t *packet, int32_t sound_id, int32_t sound_category, int32_t entity_id, float volume, float pitch)
{
	packet->type = _strdup("Entity Sound Effect");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Sound ID", sound_id);
	map_set_int32(packet->map, "Sound Category", sound_category);
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_float(packet->map, "Volume", volume);
	map_set_float(packet->map, "Pitch", pitch);
}
bool write_packet_sound_effect(struct packet_t *packet, int32_t sound_id, int32_t sound_category, int32_t effect_position_x, int32_t effect_position_y, int32_t effect_position_z, float volume, float pitch)
{
	packet->type = _strdup("Sound Effect");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Sound ID", sound_id);
	map_set_int32(packet->map, "Sound Category", sound_category);
	map_set_int32(packet->map, "Effect Position X", effect_position_x);
	map_set_int32(packet->map, "Effect Position Y", effect_position_y);
	map_set_int32(packet->map, "Effect Position Z", effect_position_z);
	map_set_float(packet->map, "Volume", volume);
	map_set_float(packet->map, "Pitch", pitch);
}
bool write_packet_player_list_header_and_footer(struct packet_t *packet, char *header, char *footer)
{
	packet->type = _strdup("Player List Header And Footer");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_string(packet->map, "Header", header);
	map_set_string(packet->map, "Footer", footer);
}
bool write_packet_collect_item(struct packet_t *packet, int32_t collected_entity_id, int32_t collector_entity_id, int32_t pickup_item_count)
{
	packet->type = _strdup("Collect Item");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Collected Entity ID", collected_entity_id);
	map_set_int32(packet->map, "Collector Entity ID", collector_entity_id);
	map_set_int32(packet->map, "Pickup Item Count", pickup_item_count);
}
bool write_packet_entity_teleport(struct packet_t *packet, int32_t entity_id, double x, double y, double z, uint8_t yaw, uint8_t pitch, bool on_ground)
{
	packet->type = _strdup("Entity Teleport");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_double(packet->map, "X", x);
	map_set_double(packet->map, "Y", y);
	map_set_double(packet->map, "Z", z);
	map_set_uint8(packet->map, "Yaw", yaw);
	map_set_uint8(packet->map, "Pitch", pitch);
	map_set_boolean(packet->map, "On Ground", on_ground);
}
bool write_packet_entity_effect(struct packet_t *packet, int32_t entity_id, int8_t effect_id, int8_t amplifier, int32_t duration, int8_t flags)
{
	packet->type = _strdup("Entity Effect");
	packet->direction = CLIENTBOUND;
	packet->map = construct_map();
	map_set_int32(packet->map, "Entity ID", entity_id);
	map_set_int8(packet->map, "Effect ID", effect_id);
	map_set_int8(packet->map, "Amplifier", amplifier);
	map_set_int32(packet->map, "Duration", duration);
	map_set_int8(packet->map, "Flags", flags);
}
