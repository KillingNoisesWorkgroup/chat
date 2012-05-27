#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>

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

void create_session(int client_socket, struct sockaddr_in *client_addres){
	session *new_session;
	
	if((new_session = malloc(sizeof(session))) == NULL){
		perror("malloc");
		exit(1);
	}
	
	sprintf(new_session->thread_info, "unnamed");
	new_session->state = SESSION_STATE_INITIAL_STATE;
	new_session->client_socket = client_socket;
	new_session->client_addres = client_addres;

	if( (pthread_create(&new_session->thread, NULL, (void*)Session, (void*)new_session)) != 0){
		perror("pthread_create");
		exit(1);
	}
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
		
		switch(packet_type){
		case PACKET_AUTH_REQUEST:
			print_log(current_session->thread_info, "Got auth packet from %s", ((packet_auth_request*)data)->login);
			authenticate(current_session, (packet_auth_request*)data);
			send_auth_response(current_session);
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
	print_log(s->thread_info, "Session terminated");
	pthread_exit(NULL);
}

void send_auth_response(session *s){
	packet_auth_response packet;
	if(!user_is_authorized(s)) return;
	packet.userid = htonl(s->user->id);
	packet_send(s->client_socket, PACKET_AUTH_RESPONSE, sizeof(packet), &packet);
}

int user_is_authorized(session *s){
	if(s->state != SESSION_STATE_AUTHORIZED) return 0;
	else return 1;
}