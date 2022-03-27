#include <uv.h>
#include "mc_packets.h"


struct client_t;

void init_game_loop(void);
void run_game_loop(void *data);

struct client_t *game_init_client(uv_tcp_t *socket);
struct client_t *game_get_client(uv_tcp_t *socket);

void game_handle_client_disconnect(uv_tcp_t *client);
void game_handle_client_packet(struct client_t *client, struct packet_t *packet);

struct game_t *construct_game(void);
