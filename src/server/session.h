#ifndef H_SERVER_SESSION_GUARD
#define H_SERVER_SESSION_GUARD

#include <pthread.h>
#include <stdint.h>

#define USERNAME_MAXSIZE 256

#define SESSION_STATE_INITIAL_STATE 0

typedef struct session{
	uint32_t userid;
	char username[USERNAME_MAXSIZE];
	pthread_t thread;
	int client_socket;
	int state;
	struct sockaddr_in *client_addres;
} session;

// Initializes new session
void create_session(int client_socket, struct sockaddr_in *client_addres);

// Thread function, where all of the packets processing take place
void* Session(void *arg);

#endif
