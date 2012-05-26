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
			
			break;
		default:
			print_log(current_session->thread_info, "Got unknown packet");
			break;
		}
		free(data);
	}
}