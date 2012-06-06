#ifndef H_CLIENT_SESSION_GUARD
#define H_CLIENT_SESSION_GUARD

#include <pthread.h>
#include <stdint.h>

#include "../shared/networking.h"

typedef struct session
{
	int s_serv;
	int s_cl;
	uint16_t port;
	uint32_t my_id;
	uint32_t host_id;
	char login[128];
	struct sockaddr_in addr;
	struct sockaddr_in connect_addr;
	pthread_t thread;
}session;

session *new_session;

void create_session(char *login, char *passw, int server_socket,struct sockaddr_in *serv_addr);

void session_client(void *arg);

void pr(char *str);
#endif

