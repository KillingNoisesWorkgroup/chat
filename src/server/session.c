#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "session.h"
#include "../shared/networking.h"
#include "database.h"

char* passw_to_hex(unsigned char * passw, int size){
	char* hex;
	char tmp[10];
	int i;
	if( (hex = malloc(size * 2 + 1)) == NULL){
		perror("malloc");
		exit(1);
	}
	hex[0] = '\0';
	for(i = 0; i < size; i++) {
		sprintf(tmp, "%02x", passw[i]);
		strcat(hex, tmp);
	}
	return hex;
}

void create_session(int client_socket, struct sockaddr_in client_addres){
	session *new_session;
	
	if((new_session = malloc(sizeof(session))) == NULL){
		perror("malloc");
		exit(1);
	}
	
	sprintf(new_session->thread_info, "unnamed");
	new_session->state = SESSION_STATE_INITIAL_STATE;
	new_session->client_socket = client_socket;
	new_session->id = database.sessions->size;
	memcpy(&new_session->client_address, &client_addres, sizeof(struct sockaddr_in));
	
	dynamic_array_add(database.sessions, new_session);
	if( (pthread_create(&new_session->thread, NULL, (void*)Session, (void*)new_session)) != 0){
		perror("pthread_create");
		exit(1);
	}
}

int session_find_id(uint32_t id, session **s){
	session* sess;
	int i, b = 0;
	if(id < 1) return -1;
	sess = NULL;
	for(i = 0; i < database.sessions->size; i++){
		if(((session*)(database.sessions->data[i]))->user->id == id){
			sess = (session*)(database.sessions->data[i]);
			b = 1;
			break;
		}
	}
	*s = sess;
	if(b) return i;
	else return -1;
}

void* Session(void *arg){
	packet_type_t packet_type;
	packet_length_t length;
	session *current_session;
	void *data;

	current_session = arg;
	
	while(1){
		
		if( !packet_recv(current_session->client_socket, &packet_type, &length, &data)){
			print_log(current_session->thread_info, "Client disconnected");
			destroy_session(current_session);
		}
		packet_debug_full(packet_type, length, data);
		switch(packet_type){
		case PACKET_AUTH_REQUEST:
			print_log(current_session->thread_info, "Got auth packet from %s", ((packet_auth_request*)data)->login);
			authenticate(current_session, (packet_auth_request*)data);
			send_auth_response(current_session);
			break;
		case PACKET_DIRECT_CONNECTION_REQUEST:
			print_log(current_session->thread_info, "Got direct connection request");
			send_client_address(current_session, data);
			break;
		case PACKET_CHAT_MESSAGE:
			print_log(current_session->thread_info, "Got chat message");
			send_chat_message(current_session, data);
			break;
		default:
			print_log(current_session->thread_info, "Got unknown packet");
			break;
		}
		free(data);
	}
}

login_entry* reg_new_user(packet_auth_request* packet, int id, char* hex){
	login_entry* login;
	login = init_login_entry(id);
	strcpy(login->login, packet->login);
	strncpy(login->passw, hex, strlen(hex));
	dynamic_array_add(database.users, login);
	return login;
}

void authenticate(session *s, packet_auth_request *packet){
	login_entry *login;
	char *hex;
	int userid;

	hex = passw_to_hex(packet->passw, ENCRYPTED_PASSWORD_LENGTH);

	if( (login_entry_find(packet->login, &login)) == -1){
		login = reg_new_user(packet, last_login_id++, hex);
		
		s->state = SESSION_STATE_AUTHORIZED;
		s->user = login;
		
		sprintf(s->thread_info, "%s %d", login->login, login->id);
		print_log(s->thread_info, "User %s(%d) was registrated", login->login, login->id);
	} else {
		if( strcmp(login->passw, hex) == 0){
			sprintf(s->thread_info, "%s %d", login->login, login->id);
			print_log(s->thread_info, "User %s(%d) successfully authenticated", login->login, login->id);
			s->state = SESSION_STATE_AUTHORIZED;
			s->user = login;
		} else {
			print_log(s->thread_info, "Authentication of user %s(%d) failed", login->login, login->id);
			destroy_session(s);
		}
	}
	free(hex);
}


void destroy_session(session* s){
	close(s->client_socket);
	dynamic_array_delete_at(database.sessions, s->id);
	print_log(s->thread_info, "Session terminated");
	pthread_exit(NULL);
}

void send_auth_response(session *s){
	packet_auth_response packet;
	if(!user_is_authorized(s)) return;
	packet.userid = htonl(s->user->id);
	packet_send(s->client_socket, PACKET_AUTH_RESPONSE, sizeof(packet), &packet);
}

void send_client_address(session *s, packet_direct_connection_request *packet){
	packet_client_address client_address;
	session *other;
	client_address.port = packet->port;
	client_address.address = s->client_address.sin_addr.s_addr;
	if(session_find_id(ntohl(packet->userid), &other) != -1){
		packet_send(other->client_socket, PACKET_CLIENT_ADDRESS, sizeof(client_address), &client_address);
		print_log(s->thread_info, "Client address sent to %s", inet_ntoa(other->client_address.sin_addr));
	}
}

void send_chat_message(session *s, packet_chat_message *packet){
	session *tmp;
	if(session_find_id(packet->receiverid, &tmp) != -1)
		packet_send(tmp->client_socket, PACKET_CHAT_MESSAGE, sizeof(packet_chat_message), packet);
}

int user_is_authorized(session *s){
	if(s->state != SESSION_STATE_AUTHORIZED) return 0;
	else return 1;
}
