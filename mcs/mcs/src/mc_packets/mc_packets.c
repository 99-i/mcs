#include "mc_packets.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson.h"
#include <assert.h>

/* returns the amount of bytes that the varint took up. the int64_t pointed to by dest receives the value of the varint */
#define READ_SIGNATURE(type, name) static uint8_t read_##name(uint8_t* data, uint8_t max, ##type* dest)
#define PLACEHOLDERS int32_t the_varint; \
char* the_string_255; \
uint16_t the_unsigned_short; \
uint8_t the_unsigned_byte; \
char* the_string_32767; \
int64_t the_long; \
char* the_identifier; \
char *the_chat; \
char *the_string_20; \
struct uuid_t the_uuid; \
char *the_string_16; \
bool the_boolean; \
double the_double; \
int32_t the_int; \
int16_t the_short; \
struct position_t the_position; \
int8_t the_byte; \
float the_float; \
char *the_string_40; \
char *the_string_256; \
char *the_string_32500; \
char *the_string; \
int64_t the_varlong; \
char *the_string_384


#define PACKET_READ_OPERATION(var, mapset) data_needle += read_##var(data + data_needle, data_size - data_needle, &the_##var); \
map_set_##mapset(packet->map, slab->fields[j].field_name, the_##var);

#define READ_STR_BODY(length) { \
	int32_t strlength; \
	size_t i = 0; \
	char* str; \
	uint8_t varint_length; \
	varint_length = read_varint(data, max, &strlength); \
	if (strlength > length) \
	{ \
		return 0; \
	} \
	str = malloc(strlength + 1); \
	for (; i < strlength; i++) \
	{ \
		if (i + varint_length > max) \
		{ \
			free(str); \
			return 0; \
		} \
		str[i] = data[i + varint_length]; \
	} \
	str[strlength] = 0; \
	*dest = _strdup(str); \
	return i + varint_length; \
}

#define READ_STR_FUNCTION(length) READ_SIGNATURE(char*, string_##length) \
READ_STR_BODY(##length)



static struct slabinfo_t
{
	size_t num_slabs;
	struct slab_t *slabs;
} slabinfo;

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


bool create_serverbound_packet(uint32_t data_size, uint8_t *data, enum estate state, struct packet_t *packet)
{
	size_t i;
	size_t j;
	struct packet_metadata_t metadata = get_packet_metadata(data_size, data);
	bool found_slab = false;
	uint32_t data_needle = 0;
	data_needle += read_varint(data, data_size, 0);
	PLACEHOLDERS;
	for (i = 0; i < slabinfo.num_slabs; i++)
	{
		if (slabinfo.slabs[i].state == state && slabinfo.slabs[i].id == metadata.packet_id)
		{
			found_slab = true;
			packet->type = _strdup(slabinfo.slabs[i].name);
			packet->map = construct_map();
			struct slab_t *slab = &slabinfo.slabs[i];
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
						map_set_short(packet->map, slab->fields[j].field_name, the_short);
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
	int8_t current_byte = 0;
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
	*dest = (data[1] << 8) | data[0];
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
	*dest = ((uint64_t)data[5] << 40) | ((uint64_t)data[4] << 32) | ((uint64_t)data[3] << 24) | ((uint64_t)data[2] << 16) | ((uint64_t)data[1] << 8) | data[0];
	return 8;
}
static uint8_t read_chat(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 262144)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_uuid(uint8_t *data, uint8_t max, struct uuid_t *dest)
{
	if (max < 16)
	{
		return 0;
	}

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
		*dest = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
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
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 40)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_string_256(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 256)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_string_32500(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 32500)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_string(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 32767)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_string_384(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 384)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_identifier(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 32767)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_string_16(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 16)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_string_20(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 20)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_string_32767(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 32767)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
}
static uint8_t read_string_255(uint8_t *data, uint8_t max, char **dest)
{
	int32_t strlength; size_t i = 0; char *str; uint8_t varint_length; varint_length = read_varint(data, max, &strlength); if (strlength > 255)
	{
		return 0;
	} str = malloc(strlength + 1); for (; i < strlength; i++)
	{
		if (i + varint_length > max)
		{
			free(str); return 0;
		} str[i] = data[i + varint_length];
	} str[strlength] = 0; *dest = _strdup(str); return i + varint_length;
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
