#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#include "session.h"
#include "conversations.h"
#include "../shared/networking.h"

uint16_t parse_id(char *str){
	int i;
	uint16_t id;
	char buf[128], tmp;
	buf[0] = 0;
	if((id = atoi(str)) == 0) return 0;
	for(i = 0; i < strlen(str) - 1; i++){
		tmp = str[i];
		if(tmp == ':'){
			id = atoi(buf);
			break;
		}
		buf[i] = tmp;
	}
	return id; 
}

char* parse_message(char *str){
	int i;
	char *message;	
	for(i = 0; i < strlen(str) - 1; str++){
		if(*str == ':') break;
	}
	message = str+1;
	return message;
}


void command_connect(char *strline){
	char* str = malloc(1);	
	char* trash = malloc(1);
	if(sscanf(strline, "%s %s", trash, str) != 2){
		printf("incorrectly entered : <address>:<port> [/connect 0.0.0.0:1111]\n");	
	}else{
		connecting(str);
  	}
}

void command_host(char *strline){
	packet_direct_connection_request packet;
	uint32_t port;
	char *trash = malloc(1);
	if(sscanf(strline, "%s %d",trash, &port) != 2){
		printf("incorrectly entered : <port> [/host 1111]\n");	
	}else{
		new_session->port = htons(port);
		hosting();
  	}
}

void command_message(char* message, uint16_t userid){
	packet_chat_message packet;
	packet.senderid = new_session->my_id;
	packet.receiverid = userid;
	memcpy(packet.message.text, parse_message(message), CHAT_MESSAGE_MAXSIZE);
	if(userid > 999)
		packet_send(new_session->s_cl,  PACKET_CHAT_MESSAGE, sizeof(packet), &packet);
	else
		packet_send(new_session->s_serv,  PACKET_CHAT_MESSAGE, sizeof(packet), &packet);
}


void create_session(char *login, char *passw, int server_socket,struct sockaddr_in *serv_addr){
	packet_auth_request auth;
	packet_type_t packet_type;
	packet_length_t length;
	void *packet;

	if((new_session = malloc(sizeof(session))) == NULL){
		perror("malloc");
		exit(1);	
	}	

	new_session->s_serv = server_socket;
  	new_session->addr = *serv_addr;
	
	strcpy(auth.login, login);
	auth.login[-1] = '\0';
	MD5((unsigned char *) passw, strlen(passw), (unsigned char*)&auth.passw);
  	packet_send(new_session->s_serv, PACKET_AUTH_REQUEST, sizeof(packet_auth_request), &auth);
  	
 	if((pthread_create(&new_session->thread, NULL, (void*)session_client, NULL)) != 0 )
  	{
    	perror("pthread: ");
    	exit(1);  
  	}
  	while(1){
	    	if( !packet_recv(new_session->s_serv, &packet_type, &length, &packet)) 
	    	{
	     		printf("Connection closed\n");
	     		exit(1);
	    	}
		switch(packet_type)
	    	{
	      	case PACKET_AUTH_RESPONSE:
			new_session->my_id = ntohl(((packet_auth_response*)(packet))->userid);
			break;
	      	case PACKET_CLIENT_ADDRESS:
	       		connect_to_client(new_session);
			break;
		case PACKET_CHAT_MESSAGE:
			printf("%d : %s\n", ((packet_chat_message*)packet)->senderid, ((packet_chat_message*)packet)->message.text);	      	
			break;
		default:
			break;
	    		}
	}
}

void session_client(void *arg)
{
 	char *command_t = malloc(1);
	char *strline = malloc(1);	
	size_t str_len;
	
	uint16_t userid;
	while(1){
		getline(&strline, &str_len, stdin);
		if((userid = parse_id(strline)) != 0){
			command_message(strline, userid);
			continue;
		}
		sscanf(strline, "%s", command_t);
		to_upper(command_t);
	      	if(strcmp(command_t, "ID") == 0){
			printf("My id = %d", new_session->my_id);
			fflush(stdout);
		}
		if(strcmp(command_t, "/HOST") == 0){
			command_host(strline);	
		}
		if(strcmp(command_t, "/CONNECT") == 0){
			command_connect(strline);
		}	
		if(strcmp(command_t, "/COMMANDS") == 0){
			printf("/id\n/add        <id>\n/host       <port>\n/connect    <address>:<port>\n");
		}

	}
}

