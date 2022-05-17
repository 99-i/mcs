#include "mcs.h"
#include "server.h"
#include <stdio.h>
#include <assert.h>
#include "types.h"
#include "util/map.h"
uv_loop_t game_loop;

static uv_timer_t timer;

#pragma region HandlePacketFunctions
static void sb_handle_handshake(struct client_t *client, struct packet_t *packet);
static void sb_handle_request(struct client_t *client, struct packet_t *packet);
static void sb_handle_ping(struct client_t *client, struct packet_t *packet);
static void sb_handle_login_start(struct client_t *client, struct packet_t *packet);
static void sb_handle_teleport_confirm(struct client_t *client, struct packet_t *packet);
static void sb_handle_query_block_nbt(struct client_t *client, struct packet_t *packet);
static void sb_handle_set_difficulty(struct client_t *client, struct packet_t *packet);
static void sb_handle_chat_message(struct client_t *client, struct packet_t *packet);
static void sb_handle_client_status(struct client_t *client, struct packet_t *packet);
static void sb_handle_client_settings(struct client_t *client, struct packet_t *packet);
static void sb_handle_tab_complete(struct client_t *client, struct packet_t *packet);
static void sb_handle_window_confirmation(struct client_t *client, struct packet_t *packet);
static void sb_handle_click_window_button(struct client_t *client, struct packet_t *packet);
static void sb_handle_close_window(struct client_t *client, struct packet_t *packet);
static void sb_handle_query_entity_nbt(struct client_t *client, struct packet_t *packet);
static void sb_handle_generate_structure(struct client_t *client, struct packet_t *packet);
static void sb_handle_keep_alive(struct client_t *client, struct packet_t *packet);
static void sb_handle_lock_difficulty(struct client_t *client, struct packet_t *packet);
static void sb_handle_player_position(struct client_t *client, struct packet_t *packet);
static void sb_handle_player_position_and_rotation(struct client_t *client, struct packet_t *packet);
static void sb_handle_player_rotation(struct client_t *client, struct packet_t *packet);
static void sb_handle_player_movement(struct client_t *client, struct packet_t *packet);
static void sb_handle_vehicle_move(struct client_t *client, struct packet_t *packet);
static void sb_handle_steer_boat(struct client_t *client, struct packet_t *packet);
static void sb_handle_pick_item(struct client_t *client, struct packet_t *packet);
static void sb_handle_craft_recipe_request(struct client_t *client, struct packet_t *packet);
static void sb_handle_player_abilities(struct client_t *client, struct packet_t *packet);
static void sb_handle_entity_action(struct client_t *client, struct packet_t *packet);
static void sb_handle_steer_vehicle(struct client_t *client, struct packet_t *packet);
static void sb_handle_set_recipe_book_state(struct client_t *client, struct packet_t *packet);
static void sb_handle_set_displayed_recipe(struct client_t *client, struct packet_t *packet);
static void sb_handle_name_item(struct client_t *client, struct packet_t *packet);
static void sb_handle_resource_pack_status(struct client_t *client, struct packet_t *packet);
static void sb_handle_select_trade(struct client_t *client, struct packet_t *packet);
static void sb_handle_set_beacon_effect(struct client_t *client, struct packet_t *packet);
static void sb_handle_held_item_change(struct client_t *client, struct packet_t *packet);
static void sb_handle_update_command_block(struct client_t *client, struct packet_t *packet);
static void sb_handle_update_command_block_minecart(struct client_t *client, struct packet_t *packet);
static void sb_handle_update_jigsaw_block(struct client_t *client, struct packet_t *packet);
static void sb_handle_update_structure_block(struct client_t *client, struct packet_t *packet);
static void sb_handle_update_sign(struct client_t *client, struct packet_t *packet);
static void sb_handle_animation(struct client_t *client, struct packet_t *packet);
static void sb_handle_spectate(struct client_t *client, struct packet_t *packet);
static void sb_handle_player_block_placement(struct client_t *client, struct packet_t *packet);
static void sb_handle_use_item(struct client_t *client, struct packet_t *packet);
#pragma endregion

static void timer_callback(uv_timer_t *handle)
{
	assert(handle == &timer);
	uv_mutex_lock(&lock);

	game->tick_count += 1;

	uv_mutex_unlock(&lock);
}
void init_game_loop(void)
{
	uv_loop_init(&game_loop);
	uv_timer_init(&game_loop, &timer);
	uv_timer_start(&timer, timer_callback, 0, 1000 / 20);
}

void run_game_loop(void *data)
{
	uv_run(&game_loop, UV_RUN_DEFAULT);
}

struct client_t *game_init_client(uv_tcp_t *socket)
{
	struct client_t *client;
	uv_mutex_lock(&lock);
	client = mcsalloc(sizeof(struct client_t));
	client->socket = socket;
	client->state = STATE_HANDSHAKING;
	client->player = 0;
	client_array_append(&game->server->clients, client);
	uv_mutex_unlock(&lock);
	return client;
}

struct client_t *game_get_client(uv_tcp_t *socket)
{
	i32 i;

	uv_mutex_lock(&lock);
	for (i = 0; i < game->server->clients.size; i++)
	{
		if (game->server->clients.fields[i]->socket == socket)
		{
			return game->server->clients.fields[i];
		}
	}
	uv_mutex_unlock(&lock);
	return 0;
}

void game_handle_client_packet(struct client_t *client, struct packet_t *packet)
{
	assert(packet->direction == SERVERBOUND);
	//printf("received %s!\n", packet->type);

	if (!strcmp(packet->type, "Handshake"))
	{
		sb_handle_handshake(client, packet);
	}
	else if (!strcmp(packet->type, "Request"))
	{
		sb_handle_request(client, packet);
	}
	else if (!strcmp(packet->type, "Ping"))
	{
		sb_handle_ping(client, packet);
	}
	else if (!strcmp(packet->type, "Login Start"))
	{
		sb_handle_login_start(client, packet);
	}
	else if (!strcmp(packet->type, "Teleport Confirm"))
	{
		sb_handle_teleport_confirm(client, packet);
	}
	else if (!strcmp(packet->type, "Query Block NBT"))
	{
		sb_handle_query_block_nbt(client, packet);
	}
	else if (!strcmp(packet->type, "Set Difficulty"))
	{
		sb_handle_set_difficulty(client, packet);
	}
	else if (!strcmp(packet->type, "Chat Message (serverbound)"))
	{
		sb_handle_chat_message(client, packet);
	}
	else if (!strcmp(packet->type, "Client Status"))
	{
		sb_handle_client_status(client, packet);
	}
	else if (!strcmp(packet->type, "Client Settings"))
	{
		sb_handle_client_settings(client, packet);
	}
	else if (!strcmp(packet->type, "Tab-Complete (serverbound)"))
	{
		sb_handle_tab_complete(client, packet);
	}
	else if (!strcmp(packet->type, "Window Confirmation (serverbound)"))
	{
		sb_handle_window_confirmation(client, packet);
	}
	else if (!strcmp(packet->type, "Click Window Button"))
	{
		sb_handle_click_window_button(client, packet);
	}
	else if (!strcmp(packet->type, "Close Window (serverbound)"))
	{
		sb_handle_close_window(client, packet);
	}
	else if (!strcmp(packet->type, "Query Entity NBT"))
	{
		sb_handle_query_entity_nbt(client, packet);
	}
	else if (!strcmp(packet->type, "Generate Structure"))
	{
		sb_handle_generate_structure(client, packet);
	}
	else if (!strcmp(packet->type, "Keep Alive (serverbound)"))
	{
		sb_handle_keep_alive(client, packet);
	}
	else if (!strcmp(packet->type, "Lock Difficulty"))
	{
		sb_handle_lock_difficulty(client, packet);
	}
	else if (!strcmp(packet->type, "Player Position"))
	{
		sb_handle_player_position(client, packet);
	}
	else if (!strcmp(packet->type, "Player Position And Rotation (serverbound)"))
	{
		sb_handle_player_position_and_rotation(client, packet);
	}
	else if (!strcmp(packet->type, "Player Rotation"))
	{
		sb_handle_player_rotation(client, packet);
	}
	else if (!strcmp(packet->type, "Player Movement"))
	{
		sb_handle_player_movement(client, packet);
	}
	else if (!strcmp(packet->type, "Vehicle Move (serverbound)"))
	{
		sb_handle_vehicle_move(client, packet);
	}
	else if (!strcmp(packet->type, "Steer Boat"))
	{
		sb_handle_steer_boat(client, packet);
	}
	else if (!strcmp(packet->type, "Pick Item"))
	{
		sb_handle_pick_item(client, packet);
	}
	else if (!strcmp(packet->type, "Craft Recipe Request"))
	{
		sb_handle_craft_recipe_request(client, packet);
	}
	else if (!strcmp(packet->type, "Player Abilities (serverbound)"))
	{
		sb_handle_player_abilities(client, packet);
	}
	else if (!strcmp(packet->type, "Entity Action"))
	{
		sb_handle_entity_action(client, packet);
	}
	else if (!strcmp(packet->type, "Steer Vehicle"))
	{
		sb_handle_steer_vehicle(client, packet);
	}
	else if (!strcmp(packet->type, "Set Recipe Book State"))
	{
		sb_handle_set_recipe_book_state(client, packet);
	}
	else if (!strcmp(packet->type, "Set Displayed Recipe"))
	{
		sb_handle_set_displayed_recipe(client, packet);
	}
	else if (!strcmp(packet->type, "Name Item"))
	{
		sb_handle_name_item(client, packet);
	}
	else if (!strcmp(packet->type, "Resource Pack Status"))
	{
		sb_handle_resource_pack_status(client, packet);
	}
	else if (!strcmp(packet->type, "Select Trade"))
	{
		sb_handle_select_trade(client, packet);
	}
	else if (!strcmp(packet->type, "Set Beacon Effect"))
	{
		sb_handle_set_beacon_effect(client, packet);
	}
	else if (!strcmp(packet->type, "Held Item Change (serverbound)"))
	{
		sb_handle_held_item_change(client, packet);
	}
	else if (!strcmp(packet->type, "Update Command Block"))
	{
		sb_handle_update_command_block(client, packet);
	}
	else if (!strcmp(packet->type, "Update Command Block Minecart"))
	{
		sb_handle_update_command_block_minecart(client, packet);
	}
	else if (!strcmp(packet->type, "Update Jigsaw Block"))
	{
		sb_handle_update_jigsaw_block(client, packet);
	}
	else if (!strcmp(packet->type, "Update Structure Block"))
	{
		sb_handle_update_structure_block(client, packet);
	}
	else if (!strcmp(packet->type, "Update Sign"))
	{
		sb_handle_update_sign(client, packet);
	}
	else if (!strcmp(packet->type, "Animation (serverbound)"))
	{
		sb_handle_animation(client, packet);
	}
	else if (!strcmp(packet->type, "Spectate"))
	{
		sb_handle_spectate(client, packet);
	}
	else if (!strcmp(packet->type, "Player Block Placement"))
	{
		sb_handle_player_block_placement(client, packet);
	}
	else if (!strcmp(packet->type, "Use Item"))
	{
		sb_handle_use_item(client, packet);
	}
	else
	{
		assert(false && "hi");
	}



}

//MEMORY LEAK HERE SOMEWHERE
void game_handle_client_disconnect(uv_tcp_t *client)
{
	struct client_t *removed_client = 0;
	i32 i;

	uv_mutex_lock(&lock);
	for (i = 0; i < game->server->clients.size; i++)
	{
		if (game->server->clients.fields[i]->socket == client)
		{
			removed_client = game->server->clients.fields[i];
			break;
		}
	}
	assert(client != 0);

	uv_close((uv_handle_t*) client, NULL);
	mcsfree(client);
	if(!removed_client) return;
	if (removed_client->player)
	{
		mcsfree(removed_client->player);
	}
	client_array_remove(&game->server->clients, i);
	mcsfree(removed_client);
	uv_mutex_unlock(&lock);

}

static void sb_handle_handshake(struct client_t *client, struct packet_t *packet)
{
	i32 next_state;
	assert((packet->direction == SERVERBOUND) && !strcmp(packet->type, "Handshake"));

	next_state = map_get_cstr(packet->map, "Next State").i32;

	switch (next_state)
	{
		case 1:
			client->state = STATE_STATUS;
			break;
		case 2:
			client->state = STATE_LOGIN;
			break;
		default:
			break;
	}

}
//memory leak somewhere here
static void sb_handle_request(struct client_t *client, struct packet_t *packet)
{
	const char *i = "{ \
			\"version\": { \
				\"name\": \"MCS 1.16.5\", \
				\"protocol\": 754 \
			}, \
			\"players\": { \
				\"max\": 100, \
				\"online\": 5 \
			}, \
			\"description\": { \
				\"text\": \"Hello world\" \
			} \
		}";
	str s = str_construct_from_cstr(i);
	struct packet_t *response_packet = construct_clientbound_packet("Response", s);
	str_destroy(&s);
	server_send_packet(client, response_packet);

	packet_free(response_packet);
}
//memory leak somewhere here
static void sb_handle_ping(struct client_t *client, struct packet_t *packet)
{
	i64 ping = map_get_cstr(packet->map, "Payload").i64;


	struct packet_t *pong = construct_clientbound_packet("Pong", ping);

	server_send_packet(client, pong);

	packet_free(pong);

}
static void sb_handle_login_start(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_teleport_confirm(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_query_block_nbt(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_set_difficulty(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_chat_message(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_client_status(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_client_settings(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_tab_complete(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_window_confirmation(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_click_window_button(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_close_window(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_query_entity_nbt(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_generate_structure(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_keep_alive(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_lock_difficulty(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_player_position(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_player_position_and_rotation(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_player_rotation(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_player_movement(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_vehicle_move(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_steer_boat(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_pick_item(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_craft_recipe_request(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_player_abilities(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_entity_action(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_steer_vehicle(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_set_recipe_book_state(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_set_displayed_recipe(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_name_item(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_resource_pack_status(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_select_trade(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_set_beacon_effect(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_held_item_change(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_update_command_block(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_update_command_block_minecart(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_update_jigsaw_block(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_update_structure_block(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_update_sign(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_animation(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_spectate(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_player_block_placement(struct client_t *client, struct packet_t *packet)
{

}
static void sb_handle_use_item(struct client_t *client, struct packet_t *packet)
{

}

