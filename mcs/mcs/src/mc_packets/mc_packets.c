#include "mc_packets.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
/* returns the amount of bytes that the varint took up. the int64_t pointed to by dest receives the value of the varint */

static uint8_t read_varint(uint8_t* data, uint8_t max, int32_t* dest);

struct packet_metadata_t get_packet_metadata(uint32_t data_length, uint8_t* data)
{
	uint8_t bytes = 0;

	int32_t id;
	int32_t length;

	bytes = read_varint(data, data_length, &length);
	bytes += read_varint(data + bytes, data_length - bytes, &id);

	return (struct packet_metadata_t)
	{
		(uint32_t)id, (uint32_t)length
	};
}



static uint8_t read_varint(uint8_t* data, uint8_t max, int32_t* dest)
{
	int32_t decoded_int = 0;
	int32_t bit_offset = 0;
	int8_t current_byte;
	uint8_t len = 0;

	do
	{
		current_byte = data[len];

		decoded_int |= (current_byte & 0b01111111) << bit_offset;

		if (bit_offset == 35)
		{
			return -1;
			break;
		}
		bit_offset += 7;
		len++;
	} while ((current_byte & 0b10000000) != 0);

	*dest = decoded_int;
	return len;
}
