#ifndef H_SERVER_SESSION_GUARD
#define H_SERVER_SESSION_GUARD

#include <pthread.h>
#include <stdint.h>

#include "../shared/networking.h"

#define USERNAME_MAXSIZE 256

#define SESSION_STATE_INITIAL_STATE 0
#define SESSION_STATE_AUTHORIZED 1

typedef struct session{
	uint32_t userid;
	char username[USERNAME_MAXSIZE];
	pthread_t thread;
	int client_socket;
	int state;
	struct sockaddr_in *client_addres;
	char thread_info[128];
} session;

// Initializes new session
void create_session(int client_socket, struct sockaddr_in *client_addres);

// Thread function, where all of the packets processing take place
void* Session(void *arg);

// Authenticates user. On success, changes session state, uesrid and username, destroys session on failure
void authenticate(session *s, packet_auth_request *packet);

// Checks if user is authorized
int user_is_authorized(session *s);


// Sends packet_auth_response
void send_auth_response(session *s);

#endif
