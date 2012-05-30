#ifndef H_CLIENTS_CONVERSATION_GUARD
#define H_CLIENTS_CONVERSATION_GUARD

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#include "../shared/networking.h"


typedef struct conversation
{
	int socket_client;
	uint16_t port;
	int socket;
} conversation;  

void to_upper(char *str);

void direct_packet_send();

void direct_packet_recv();

// direct_connecting to host
void direct_connection(struct sockaddr_in addr);
// start hosting to client
void hosting(uint16_t port, uint32_t userid);
#endif
