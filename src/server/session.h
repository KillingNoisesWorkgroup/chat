#ifndef H_SERVER_SESSION_GUARD
#define H_SERVER_SESSION_GUARD

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../shared/networking.h"
#include "login_entry.h"

#define USERNAME_MAXSIZE 256

#define SESSION_STATE_INITIAL_STATE 0
#define SESSION_STATE_AUTHORIZED 1

typedef struct session{
	login_entry *user;
	pthread_t thread;
	int client_socket;
	int state;
	struct sockaddr_in client_address;
	char thread_info[128];
	int id;
} session;

// Initializes new session
void create_session(int client_socket, struct sockaddr_in client_addres);

// Searches login entry login in sessions array and places it in *s
// Return -1 on failure, index in sessions array on success
int session_find_id(uint32_t id, session **s);

// Does all the free's after session is finished
void destroy_session(session *s);

// Thread function, where all of the packets processing take place
void* Session(void *arg);

// Authenticates user. On success, changes session state, uesrid and username, destroys session on failure
void authenticate(session *s, packet_auth_request *packet);

// Checks if user is authorized
int user_is_authorized(session *s);

// Registers new user
login_entry* reg_new_user(packet_auth_request* packet, int id, char* hex);


// Sends packet_auth_response
void send_auth_response(session *s);

// Sends packet_client_address
void send_client_address(session *s, packet_direct_connection_request *packet);

// Sends packet_chat_message
void send_chat_message(session *s, packet_chat_message *packet);

#endif
