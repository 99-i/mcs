#include "mc_packets.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson/cjson.h"
#include <assert.h>
#include "mcs.h"
#include "util/buf.h"

#undef uuid_t
#define READY_SLICED_BUFFER() buf_destroy(&sliced); \
sliced = buf_slice_from_buf(b, data_needle, b.size)

struct slabinfo_t slabinfo;
static uint8_t read_varint(buf b, int32_t *dest);
static uint8_t read_unsigned_short(buf b, uint16_t *dest);
static uint8_t read_unsigned_byte(buf b, uint8_t *dest);
static uint8_t read_long(buf b, int64_t *dest);
static uint8_t read_unsigned_long(buf b, uint64_t *dest);
static uint8_t read_uuid(buf b, struct uuid_t *dest);
static uint8_t read_boolean(buf b, _Bool *dest);
static uint8_t read_double(buf b, double *dest);
static uint8_t read_int(buf b, int32_t *dest);
static uint8_t read_short(buf b, int16_t *dest);
static uint8_t read_position(buf b, struct position_t *dest);
static uint8_t read_byte(buf b, int8_t *dest);
static uint8_t read_float(buf b, float *dest);
static uint8_t read_string(buf b, str *dest);
static uint8_t read_varlong(buf b, int64_t *dest);
struct packet_metadata_t get_packet_metadata(buf b)
{
	uint8_t bytes = 0;
	buf id_sliced_buf;
	int64_t id;
	int64_t length;

	bytes = read_varint(b, &length);
	id_sliced_buf = buf_slice_from_buf(b, bytes, b.size);
	bytes += read_varint(id_sliced_buf, &id);

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
	char* jsonstr;
	cJSON *all;
	cJSON *current_slab;
	cJSON *fields;
	int num_slabs;
	int num_fields;
	size_t i;
	size_t j;
	struct slab_t slab;
	struct field_t field;
	slurp_file_to_cstr("slabs.json", &jsonstr);

	all = cJSON_Parse(jsonstr);
	num_slabs = cJSON_GetArraySize(all);

	current_slab = all->child;

	slabinfo.slabs = slab_array_construct(num_slabs);

	for (i = 0; i < num_slabs; i++)
	{
		slab.direction = bound_to_to_direction(cJSON_GetObjectItem(current_slab, "boundTo")->valuestring);
		slab.state = state_str_to_state(cJSON_GetObjectItem(current_slab, "state")->valuestring);
		slab.id = cJSON_GetObjectItem(current_slab, "id")->valueint;
		slab.name = cJSON_GetObjectItem(current_slab, "name")->valuestring;

		fields = cJSON_GetObjectItem(current_slab, "fields");

		num_fields = cJSON_GetArraySize(fields);

		slab.fields = slab_field_array_construct(num_fields);

		for (j = 0; j < num_fields; j++)
		{
			field.field_name = cJSON_GetObjectItem(cJSON_GetArrayItem(fields, j), "fieldName")->valuestring;
			field.type = field_str_to_field_type(cJSON_GetObjectItem(cJSON_GetArrayItem(fields, j), "fieldType")->valuestring);
			slab_field_array_append(&slab.fields, field);
		}

		slab_array_append(&slabinfo.slabs, slab);
		current_slab = current_slab->next;
	}




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
		return FT_STRING;
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
bool should_wraparound(buf b, struct wraparound_t *cutoff)
{
	int32_t length;
	uint8_t length_size;
	length_size = read_varint(b, &length);

	if (b.size - length_size > length)
	{
		cutoff->cutoff = length_size + length;
		cutoff->first_size = length_size + length;
		cutoff->second_size = b.size - (length_size + length);
		return true;
	}

	return false;
}
bool create_serverbound_packet(buf b, enum estate state, struct packet_t *packet)
{
	size_t i;
	size_t j;
	buf sliced;
	struct packet_metadata_t metadata = get_packet_metadata(b);
	bool found_slab = false;
	uint32_t data_needle = 0;
	data_needle += read_varint(b, 0);
	sliced = buf_slice_from_buf(b, data_needle, b.size);
	data_needle += read_varint(sliced, 0);


	str the_string;
	uint16_t the_unsigned_short;
	uint8_t the_unsigned_byte;
	int64_t the_long;
	struct uuid_t the_uuid;
	bool the_boolean;
	double the_double;
	int32_t the_int;
	int16_t the_short;
	struct position_t the_position;
	int8_t the_byte;
	float the_float;

	for (i = 0; i < slabinfo.slabs.size; i++)
	{
		if (slabinfo.slabs.fields[i].state == state && slabinfo.slabs.fields[i].id == metadata.packet_id && slabinfo.slabs.fields[i].direction == SERVERBOUND)
		{
			found_slab = true;
			struct slab_t *slab = &slabinfo.slabs.fields[i];
			packet->type = _strdup(slab->name);
			packet->map = construct_map();
			packet->direction = SERVERBOUND;

			for (j = 0; j < slab->fields.size; j++)
			{
				switch (slab->fields.fields[j].type)
				{
					case FT_VARINT:
						READY_SLICED_BUFFER();
						data_needle += read_varint(sliced, &the_int);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_i32(the_int));
						break;
					case FT_UNSIGNED_SHORT:
						READY_SLICED_BUFFER();
						data_needle += read_unsigned_short(sliced, &the_unsigned_short);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_u16(the_unsigned_short));
						break;
					case FT_UNSIGNED_BYTE:
						READY_SLICED_BUFFER();
						data_needle += read_unsigned_byte(sliced, &the_unsigned_byte);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_u8(the_unsigned_byte));
						break;
					case FT_LONG:
						READY_SLICED_BUFFER();
						data_needle += read_long(sliced, the_long);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_i64(the_long));
						break;
					case FT_UUID:
						READY_SLICED_BUFFER();
						data_needle += read_uuid(sliced, &the_uuid);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_uuid(the_uuid));
						break;
					case FT_BOOLEAN:
						READY_SLICED_BUFFER();
						data_needle += read_boolean(sliced, &the_boolean);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_u8(the_boolean));
						break;
					case FT_DOUBLE:
						READY_SLICED_BUFFER();
						data_needle += read_double(sliced, &the_double);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_double(the_double));
						break;
					case FT_ANGLE:
						READY_SLICED_BUFFER();
						data_needle += read_unsigned_byte(sliced, &the_unsigned_byte);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_u8(the_unsigned_byte));
						break;
					case FT_INT:
						READY_SLICED_BUFFER();
						data_needle += read_int(sliced, &the_int);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_i32(the_int));
						break;
					case FT_SHORT:
						READY_SLICED_BUFFER();
						data_needle += read_short(sliced, &the_short);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_i16(the_short));
						break;
					case FT_POSITION:
						READY_SLICED_BUFFER();
						data_needle += read_position(sliced, &the_position);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_pos(the_position));
						break;
					case FT_BYTE:
						READY_SLICED_BUFFER();
						data_needle += read_byte(sliced, &the_byte);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_i8(the_byte));
						break;
					case FT_FLOAT:
						READY_SLICED_BUFFER();
						data_needle += read_float(sliced, &the_float);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_float(the_float));
						break;
					case FT_STRING:
						READY_SLICED_BUFFER();
						data_needle += read_string(sliced, the_string);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_str(the_string));
						break;
					case FT_VARLONG:
						READY_SLICED_BUFFER();
						data_needle += read_varlong(sliced, the_long);
						map_set(packet->map, slab->fields.fields[j].field_name, mv_i64(the_long));
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
static uint8_t read_varint(buf b, int32_t *dest)
{
	int32_t decoded_int = 0;
	int32_t bit_offset = 0;
	uint8_t current_byte = 0;
	uint8_t len = 0;

	do
	{
		current_byte = b.data[len];

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
static uint8_t read_unsigned_short(buf b, uint16_t *dest)
{
	if (b.size < 2)
	{
		return 0;
	}
	*dest = 0;
	*dest = (b.data[0] << 8) | b.data[1];
	return 2;
}
static uint8_t read_unsigned_byte(buf b, uint8_t *dest)
{
	if (b.size < 1)
	{
		return 0;
	}
	*dest = b.data[0];
	return 1;
}
static uint8_t read_long(buf b, int64_t *dest)
{
	if (b.size < 8)
	{
		return 0;
	}
	*dest = 0;
	*dest = (((uint64_t)b.data[0]) << 56) |
			(((uint64_t)b.data[1]) << 48) |
			(((uint64_t)b.data[2]) << 40) |
			(((uint64_t)b.data[3]) << 32) |
			(((uint64_t)b.data[4]) << 24) |
			(((uint64_t)b.data[5]) << 16) |
			(((uint64_t)b.data[6]) << 8) |
			(((uint64_t)b.data[7]) << 0);
	return 8;
}
static uint8_t read_uuid(buf b, struct uuid_t *dest)
{
	uint64_t high;
	uint64_t low;
	buf sliced;
	if (b.size < 16)
	{
		return 0;
	}
	high = read_unsigned_long(b, &high);
	sliced = buf_slice_from_buf(b, high, b.size);
	low = read_unsigned_long(sliced, &low);

	dest->high = high;
	dest->low = low;
	return 16;
}
static uint8_t read_boolean(buf b, _Bool *dest)
{
	if (b.size < 1)
	{
		return 0;
	}
	if (dest)
	{
		*dest = !!b.data[0];
	}
	return 1;
}
static uint8_t read_double(buf b, double *dest)
{
	uint64_t holder;
	if (b.size < 8)
	{
		return 0;
	}
	read_unsigned_long(b, &holder);

	if (dest)
	{
		*dest = *(double *)&holder;
	}
	return 8;
}
static uint8_t read_int(buf b, int32_t *dest)
{
	if (b.size < 4)
	{
		return 0;
	}
	if (dest)
	{
		*dest = 0;
		*dest = (b.data[0] << 24) | (b.data[1] << 16) | (b.data[2] << 8) | b.data[3];
	}
	return 4;
}
static uint8_t read_short(buf b, int16_t *dest)
{
	if (b.size < 2)
	{
		return 0;
	}
	if (dest)
	{

		*dest = 0;
		*dest = (b.data[1] << 8) | b.data[0];
	}
	return 2;
}
static uint8_t read_position(buf b, struct position_t *dest)
{
	uint64_t l;
	if (b.size < 8)
	{
		return 0;
	}
	read_unsigned_long(b, &l);
	if (dest)
	{

		dest->x = (l >> 38);
		dest->y = (l & 0xFF);
		dest->z = (l >> 12) & 0x3FFFFFF;
	}

	return 8;
}
static uint8_t read_unsigned_long(buf b, uint64_t *dest)
{
	int64_t l;
	if (b.size < 8)
	{
		return 0;
	}
	read_long(b, &l);

	if (dest)
	{
		*dest = *((uint64_t *)&l);
	}
	return 8;
}
static uint8_t read_byte(buf b, int8_t *dest)
{
	int8_t l;
	if (b.size < 1)
	{
		return 0;
	}
	read_unsigned_byte(b, &l);

	if (dest)
	{
		*dest = *((int8_t *)&l);
	}
	return 1;
}
static uint8_t read_float(buf b, float *dest)
{

}
static uint8_t read_string(buf b, str *dest)
{
	int32_t strlength;
	size_t i;
	char *str;
	uint8_t varint_length;
	varint_length = read_varint(b, &strlength);
	if (strlength > 32767)
	{
		return 0;
	}
	str = malloc(strlength + 1);
	for (i = 0; i < strlength; i++)
	{
		if (i + varint_length > b.size)
		{
			free(str);
			return 0;
		}
		str[i] = b.data[i + varint_length];
	}
	str[strlength] = 0;
	*dest = str_construct_from_cstr(str);
	return i + varint_length;
}
static uint8_t read_varlong(buf b, int64_t *dest)
{
	int64_t decoded_long = 0;
	int32_t bit_offset = 0;
	int8_t current_byte = 0;
	uint8_t len = 0;

	do
	{
		current_byte = b.data[len];

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
struct packet_t* construct_clientbound_packet(str packet_type, ...)
{
	struct packet_t* packet = malloc(sizeof(struct packet_t));
	struct slab_t* current_slab = 0;
	struct field_t* current_field = 0;

	va_list argp;

	va_start(argp, packet_type);

	size_t i;

	packet->direction = CLIENTBOUND;

	for (i = 0; i < slabinfo.slabs.size; i++)
	{
		if (!strcmp(packet_type, slabinfo.slabs.fields[i].name))
		{
			current_slab = &slabinfo.slabs.fields[i];
		}
	}

	assert(current_slab != 0);
	assert(current_slab->direction == CLIENTBOUND);

	packet->type = packet_type;
	packet->map = construct_map();

	i32 int32;
	u16 uint16;
	u8 uint8;
	str ch;

	for (i = 0; i < current_slab->fields.size; i++)
	{
		current_field = &current_slab->fields.fields[i];

		switch (current_field->type)
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
			ch = va_arg(argp, str);
			map_set(packet->map, current_field->field_name, mv_str(ch));
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
buf make_varint(i32 varint)
{
	buf buf = buf_construct();
	size_t i = 0;
	while (true)
	{
		if (i == 5)
		{
			break;
		}
		if ((varint & ~0x7f) == 0)
		{
			buf_append(&buf, varint);
			break;
		}

		buf_append(&buf, (varint & 0x7f) | 0x80);
		varint >>= 7;
		i++;
	}

	return buf;


}
buf make_u16(u16 unsigned_short)
{
	buf buf = buf_construct();
	buf_append(&buf, (unsigned_short & 0xff00) >> 8);
	buf_append(&buf, (unsigned_short & 0xff));

	return buf;
}
buf make_u8(u8 unsigned_byte)
{
	buf buf = buf_construct();

	buf_append(&buf, unsigned_byte);

	return buf;
}
buf make_i64(i64 l)
{
	buf buf = buf_construct();
	buf_append(&buf, (l & 0xFF00000000000000) >> 56);
	buf_append(&buf, (l & 0xFF000000000000) >> 48);
	buf_append(&buf, (l & 0xFF0000000000) >> 40);
	buf_append(&buf, (l & 0xFF00000000) >> 32);
	buf_append(&buf, (l & 0xFF000000) >> 24);
	buf_append(&buf, (l & 0xFF0000) >> 16);
	buf_append(&buf, (l & 0xFF00) >> 8);
	buf_append(&buf, (l & 0xFF));
	return buf;
}
buf make_uuid(struct uuid_t uuid)
{
	buf buf = buf_construct();
	buf_append(&buf, (uuid.high & 0xFF00000000000000) >> 56);
	buf_append(&buf, (uuid.high & 0xFF000000000000) >> 48);
	buf_append(&buf, (uuid.high & 0xFF0000000000) >> 40);
	buf_append(&buf, (uuid.high & 0xFF00000000) >> 32);
	buf_append(&buf, (uuid.high & 0xFF000000) >> 24);
	buf_append(&buf, (uuid.high & 0xFF0000) >> 16);
	buf_append(&buf, (uuid.high & 0xFF00) >> 8);
	buf_append(&buf, (uuid.high & 0xFF));
	buf_append(&buf, (uuid.low & 0xFF00000000000000) >> 56);
	buf_append(&buf, (uuid.low & 0xFF000000000000) >> 48);
	buf_append(&buf, (uuid.low & 0xFF0000000000) >> 40);
	buf_append(&buf, (uuid.low & 0xFF00000000) >> 32);
	buf_append(&buf, (uuid.low & 0xFF000000) >> 24);
	buf_append(&buf, (uuid.low & 0xFF0000) >> 16);
	buf_append(&buf, (uuid.low & 0xFF00) >> 8);
	buf_append(&buf, (uuid.low & 0xFF));

	
	

	return buf;
}
buf make_boolean(bool boolean)
{
	buf buf = buf_construct();
	

	buf_append(&buf, boolean);

	
	

	return buf;
}
buf make_double(double d)
{
	buf buf = buf_construct();
	
	buf_append(&buf, ((*(uint64_t *)&d) & 0xFF00000000000000) >> 56);
	buf_append(&buf, ((*(uint64_t *)&d) & 0xFF000000000000) >> 48);
	buf_append(&buf, ((*(uint64_t *)&d) & 0xFF0000000000) >> 40);
	buf_append(&buf, ((*(uint64_t *)&d) & 0xFF00000000) >> 32);
	buf_append(&buf, ((*(uint64_t *)&d) & 0xFF000000) >> 24);
	buf_append(&buf, ((*(uint64_t *)&d) & 0xFF0000) >> 16);
	buf_append(&buf, ((*(uint64_t *)&d) & 0xFF00) >> 8);
	buf_append(&buf, ((*(uint64_t *)&d) & 0xFF));
	
	
	return buf;
}
buf make_angle(u8 angle)
{
	buf buf = buf_construct();
	

	buf_append(&buf, angle);
	
	

	return buf;
}
buf make_i32(i32 integer)
{
	buf buf = buf_construct();
	

	buf_append(&buf, (integer & 0xff000000) >> 24);
	buf_append(&buf, (integer & 0xff0000) >> 16);
	buf_append(&buf, (integer & 0xff00) >> 8);
	buf_append(&buf, (integer & 0xff));

	
	

	return buf;
}
buf make_i16(i16 s)
{
	buf buf = buf_construct();
	u8 *data;

	buf_append(&buf, (s & 0xff00) >> 8);
	buf_append(&buf, (s & 0xff));

	
	

	return buf;

}
buf make_position(struct position_t pos)
{
	i64 int_pos = 0;

	pos.x &= 0x3ffffff;
	pos.z &= 0x3ffffff;
	pos.y &= 0xfff;
	int_pos |= (int64_t)pos.x << (64 - 26);
	int_pos |= (int64_t)pos.z << (64 - 26 - 26);
	int_pos |= (int64_t)pos.y;


	return make_i64(int_pos);
}
buf make_i8(i8 byte)
{
	buf buf = buf_construct();
	buf_append(&buf, (byte & 0xff));
	return buf;
}
buf make_float(float f)
{
	uint64_t data;
	data = *((uint64_t*)&f);

	return make_i64(data);
}
buf make_string(str str)
{
	size_t i;
	buf b = buf_construct();
	char* cstr = str_cstr(str);
	buf varint_buf = make_varint(str_length(str));

	buf_append_buf(&b, varint_buf);
	
	buf_destroy(&varint_buf);
	for (i = 0; i < str_length(str); i++)
	{
		buf_append(&b, str_get(str, i));
	}

	return b;
}
buf make_varlong(i64 varlong)
{
	buf buf = buf_construct();
	
	size_t i = 0;
	while (true)
	{
		if (i == 10)
		{
			break;
		}
		if ((varlong & ~0x7f) == 0)
		{
			buf_append(&buf, varlong);
			break;
		}

		buf_append(&buf, (varlong & 0x7f) | 0x80);
		varlong >>= 7;
		i++;
	}

	
	
	return buf;
}
void slurp_file_to_cstr(const char* file_name, char** cstr)
{
	FILE* fp = fopen(file_name, "r");
	long f_size;
	size_t new_len;

	if (!fp)
	{
		*cstr = 0;
		return;
	}

	fseek(fp, 0, SEEK_END);

	f_size = ftell(fp);

	if (f_size == -1)
	{
		*cstr = 0;
		return;
	}

	*cstr = malloc(sizeof(char) * (f_size + 1));

	fseek(fp, 0, SEEK_SET);

	new_len = fread(*cstr, sizeof(char), f_size, fp);

	if (!new_len)
	{
		*cstr = 0;
		return;
	}

	*cstr[++new_len] = 0;

	fclose(fp);

}
