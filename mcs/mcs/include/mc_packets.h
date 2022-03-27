#pragma once
#include <stdint.h>
#include "util.h"


struct packet_metadata_t
{
	uint8_t packet_id;
	uint16_t length;
};



enum efield_type
{
	FT_VARINT,
	FT_UNSIGNED_SHORT,
	FT_UNSIGNED_BYTE,
	FT_LONG,
	FT_CHAT,
	FT_UUID,
	FT_IDENTIFIER,
	FT_BOOLEAN,
	FT_DOUBLE,
	FT_ANGLE,
	FT_INT,
	FT_SHORT,
	FT_POSITION,
	FT_BYTE,
	FT_FLOAT,
	FT_STRING_16,
	FT_STRING_20,
	FT_STRING_40,
	FT_STRING_255,
	FT_STRING_256,
	FT_STRING_384,
	FT_STRING_32500,
	FT_STRING_32767,
	FT_STRING,
	FT_VARLONG,
	FT_CONDITIONAL,
	FT_ARRAY_OF_SIZE
};



enum econdition
{
	CONDITION_EQUALS
};

enum epacket_direction
{
	SERVERBOUND,
	CLIENTBOUND
};
struct position_t
{
	int32_t x;
	int16_t y;
	int32_t z;
};
struct uuid_t
{
	uint64_t high;
	uint64_t low;
};

struct slab_t
{
	char *name;
	uint32_t id;
	enum estate state;
	enum epacket_direction direction;
	uint32_t num_fields;
	struct field_t
	{
		char *field_name;
		struct field_info_t
		{
			enum efield_type type;

			union
			{
				struct condition_t
				{
					enum econdition condition_type;
					enum efield_type result;

					union
					{
						struct
						{
							char *field_one;
							char *field_two;
						} equals;
					} comparison;
				} condition;
				struct array_t
				{
					char *size_field;
					enum efield_type type_of_size_field;
					enum efield_type type_of_elements;
				} array;
			} field;
		} fieldinfo;
	}*fields;
};

struct packet_metadata_t get_packet_metadata(uint32_t data_length, uint8_t *data);

struct packet_t
{
	enum epacket_direction direction;
	char *type;
	map_t map;
};

void construct_slabs(void);

struct wraparound_t
{
	uint32_t cutoff;
	uint32_t first_size;
	uint32_t second_size;
};
bool should_wraparound(uint8_t *data, uint32_t data_size, struct wraparound_t *cutoff);

bool create_serverbound_packet(uint32_t data_size, uint8_t *data, enum estate state, struct packet_t *packet);

bool write_packet_response(struct packet_t *packet, char *json_response);
bool write_packet_pong(struct packet_t *packet, int64_t payload);
bool write_packet_disconnect_login(struct packet_t *packet, char *reason);
bool write_packet_login_success(struct packet_t *packet, struct uuid_t uuid, char *username);
bool write_packet_set_compression(struct packet_t *packet, int32_t threshold);
bool write_packet_spawn_entity(struct packet_t *packet, int32_t entity_id, struct uuid_t object_uuid, int32_t type, double x, double y, double z, uint8_t pitch, uint8_t yaw, int32_t data, int16_t velocity_x, int16_t velocity_y, int16_t velocity_z);
bool write_packet_spawn_experience_orb(struct packet_t *packet, int32_t entity_id, double x, double y, double z, int16_t count);
bool write_packet_spawn_living_entity(struct packet_t *packet, int32_t entity_id, struct uuid_t entity_uuid, int32_t type, double x, double y, double z, uint8_t yaw, uint8_t pitch, uint8_t head_pitch, int16_t velocity_x, int16_t velocity_y, int16_t velocity_z);
bool write_packet_spawn_player(struct packet_t *packet, int32_t entity_id, struct uuid_t player_uuid, double x, double y, double z, uint8_t yaw, uint8_t pitch);
bool write_packet_entity_animation(struct packet_t *packet, int32_t entity_id, uint8_t animation);
bool write_packet_acknowledge_player_digging(struct packet_t *packet, struct position_t location, int32_t block, int32_t status, bool successful);
bool write_packet_block_break_animation(struct packet_t *packet, int32_t entity_id, struct position_t location, int8_t destroy_stage);
bool write_packet_block_action(struct packet_t *packet, struct position_t location, uint8_t action_id_(byte_1), uint8_t action_param_(byte_2), int32_t block_type);
bool write_packet_block_change(struct packet_t *packet, struct position_t location, int32_t block_id);
bool write_packet_server_difficulty(struct packet_t *packet, uint8_t difficulty, bool difficulty_locked);
bool write_packet_chat_message(struct packet_t *packet, char *json_data, int8_t position, struct uuid_t sender);
bool write_packet_window_confirmation(struct packet_t *packet, int8_t window_id, int16_t action_number, bool accepted);
bool write_packet_close_window(struct packet_t *packet, uint8_t window_id);
bool write_packet_window_property(struct packet_t *packet, uint8_t window_id, int16_t property, int16_t value);
bool write_packet_set_cooldown(struct packet_t *packet, int32_t item_id, int32_t cooldown_ticks);
bool write_packet_named_sound_effect(struct packet_t *packet, char *sound_name, int32_t sound_category, int32_t effect_position_x, int32_t effect_position_y, int32_t effect_position_z, float volume, float pitch);
bool write_packet_disconnect_play(struct packet_t *packet, char *reason);
bool write_packet_unload_chunk(struct packet_t *packet, int32_t chunk_x, int32_t chunk_z);
bool write_packet_change_game_state(struct packet_t *packet, uint8_t reason, float value);
bool write_packet_open_horse_window(struct packet_t *packet, int8_t window_id, int32_t number_of_slots, int32_t entity_id);
bool write_packet_keep_alive(struct packet_t *packet, int64_t keep_alive_id);
bool write_packet_effect(struct packet_t *packet, int32_t effect_id, struct position_t location, int32_t data, bool disable_relative_volume);
bool write_packet_entity_position(struct packet_t *packet, int32_t entity_id, int16_t delta_x, int16_t delta_y, int16_t delta_z, bool on_ground);
bool write_packet_entity_position_and_rotation(struct packet_t *packet, int32_t entity_id, int16_t delta_x, int16_t delta_y, int16_t delta_z, uint8_t yaw, uint8_t pitch, bool on_ground);
bool write_packet_entity_rotation(struct packet_t *packet, int32_t entity_id, uint8_t yaw, uint8_t pitch, bool on_ground);
bool write_packet_entity_movement(struct packet_t *packet, int32_t entity_id);
bool write_packet_vehicle_move(struct packet_t *packet, double x, double y, double z, float yaw, float pitch);
bool write_packet_open_book(struct packet_t *packet, int32_t hand);
bool write_packet_open_window(struct packet_t *packet, int32_t window_id, int32_t window_type, char *window_title);
bool write_packet_open_sign_editor(struct packet_t *packet, struct position_t location);
bool write_packet_craft_recipe_response(struct packet_t *packet, int8_t window_id, char *recipe);
bool write_packet_player_abilities(struct packet_t *packet, int8_t flags, float flying_speed, float field_of_view_modifier);
bool write_packet_player_position_and_look(struct packet_t *packet, double x, double y, double z, float yaw, float pitch, int8_t flags, int32_t teleport_id);
bool write_packet_remove_entity_effect(struct packet_t *packet, int32_t entity_id, int8_t effect_id);
bool write_packet_resource_pack_send(struct packet_t *packet, char *url, char *hash);
bool write_packet_entity_head_look(struct packet_t *packet, int32_t entity_id, uint8_t head_yaw);
bool write_packet_select_advancement_tab(struct packet_t *packet, bool has_id, char *optional_identifier);
bool write_packet_camera(struct packet_t *packet, int32_t camera_id);
bool write_packet_held_item_change(struct packet_t *packet, int8_t slot);
bool write_packet_update_view_position(struct packet_t *packet, int32_t chunk_x, int32_t chunk_z);
bool write_packet_update_view_distance(struct packet_t *packet, int32_t view_distance);
bool write_packet_spawn_position(struct packet_t *packet, struct position_t location);
bool write_packet_display_scoreboard(struct packet_t *packet, int8_t position, char *score_name);
bool write_packet_attach_entity(struct packet_t *packet, int32_t attached_entity_id, int32_t holding_entity_id);
bool write_packet_entity_velocity(struct packet_t *packet, int32_t entity_id, int16_t velocity_x, int16_t velocity_y, int16_t velocity_z);
bool write_packet_set_experience(struct packet_t *packet, float experience_bar, int32_t level, int32_t total_experience);
bool write_packet_update_health(struct packet_t *packet, float health, int32_t food, float food_saturation);
bool write_packet_time_update(struct packet_t *packet, int64_t world_age, int64_t time_of_day);
bool write_packet_entity_sound_effect(struct packet_t *packet, int32_t sound_id, int32_t sound_category, int32_t entity_id, float volume, float pitch);
bool write_packet_sound_effect(struct packet_t *packet, int32_t sound_id, int32_t sound_category, int32_t effect_position_x, int32_t effect_position_y, int32_t effect_position_z, float volume, float pitch);
bool write_packet_player_list_header_and_footer(struct packet_t *packet, char *header, char *footer);
bool write_packet_collect_item(struct packet_t *packet, int32_t collected_entity_id, int32_t collector_entity_id, int32_t pickup_item_count);
bool write_packet_entity_teleport(struct packet_t *packet, int32_t entity_id, double x, double y, double z, uint8_t yaw, uint8_t pitch, bool on_ground);
bool write_packet_entity_effect(struct packet_t *packet, int32_t entity_id, int8_t effect_id, int8_t amplifier, int32_t duration, int8_t flags);

