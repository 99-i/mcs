#pragma once
#include <openssl/rsa.h>
#define VERIFY_TOKEN_LENGTH 4

typedef struct server_auth
{
	//rsa keypair
	//server's 
	RSA *rsa;
} server_auth;

server_auth *auth_init();
void auth_destroy(server_auth *auth);



typedef enum client_auth_state
{
	AWAITING_LOGIN_START,
	SENT_ENCRYPTION_REQUEST,
	
} client_auth_state;

typedef struct client_auth
{
	client_auth_state state;

} client_auth;
