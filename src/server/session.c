#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>

#include "session.h"
#include "../shared/networking.h"

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
			break;
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

void authenticate(session *s, packet_auth_request *packet){
	
}

void send_auth_response(session *s){
	packet_auth_response packet;
	if(!user_is_authorized(s)) return;
	packet.userid = htonl(s->userid);
	packet_send(s->client_socket, PACKET_AUTH_RESPONSE, sizeof(packet), &packet);
}

int user_is_authorized(session *s){
	if(s->state != SESSION_STATE_AUTHORIZED) return 0;
	else return 1;
}