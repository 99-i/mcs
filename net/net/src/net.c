#include "net.h"
#include <stdio.h>
#include <windows.h>
#include <WinSock2.h>

void net_init(void)
{
	WSADATA wsa_data;
	int result;

	result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if(!result)
	{
		fprintf(stderr, "Failed to initialize WSA.");
	}

}
