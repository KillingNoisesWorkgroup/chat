#ifndef H_CLIENTS_CONVERSATION_GUARD
#define H_CLIENTS_CONVERSATION_GUARD

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#include "session.h"
#include "../shared/networking.h"


typedef struct conversation
{
	int socket_client;
	uint16_t port;
	int socket;
} conversation;
  
pthread_mutex_t address_lock;

void to_upper(char *str);

void hosting(uint16_t port);

void* HostThread(void *arg);

void connect_to_client(void *arg);

void* ClientConnectionThread(void *arg);

void recving_packets();
#endif
