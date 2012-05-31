#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#include "session.h"
#include "conversations.h"
#include "../shared/networking.h"

void command_host(int server_socket)
{
	packet_direct_connection_request packet;
	int port, userid;
	if(scanf("%d %d", &port, &userid) != 2){
		printf("Not correct attr\n");	
	}else{
		packet.userid = htonl((uint32_t)userid);
		packet.port = htons((uint16_t)port);
		
		packet_send(server_socket, PACKET_DIRECT_CONNECTION_REQUEST, sizeof(packet), &packet);
		hosting(packet.port);
  	}
}

void command_message(int server_socket)
{
	packet_chat_message packet;
	char *str = NULL;
	size_t str_len;
	uint32_t id;
	if(scanf("%d ", &id) != 1 ){
		printf("Not correct attr\n");	
	}else{
		getline(&str, &str_len, stdin);
		str++;		
		packet.senderid = new_session->my_id;
		packet.receiverid = id;
		memcpy(packet.message.text, str, CHAT_MESSAGE_MAXSIZE);	
		printf("Debug senderid = %d\nDebug receiverid = %d\nDebug text = %s", packet.senderid, packet.receiverid, packet.message.text);
		packet_send(server_socket, PACKET_DIRECT_CONNECTION_REQUEST, sizeof(packet), &packet);
  	}
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
	auth.login[PLAYER_NAME_MAXSIZE-1] = '\0';
	MD5((unsigned char *) passw, strlen(passw), (unsigned char*)&auth.passw);
  	packet_send(server_socket, PACKET_AUTH_REQUEST, sizeof(packet_auth_request), &auth);
  	
 	if((pthread_create(&new_session->thread, NULL, (void*)session_client, NULL)) != 0 )
  	{
    	perror("pthread: ");
    	exit(1);  
  	}
  	while(1){
	    	if( !packet_recv(server_socket, &packet_type, &length, &packet)) 
	    	{
	     		printf("Connection closed\n");
	     		exit(1);
	    	}
		packet_debug_full(packet_type, length, packet);
		switch(packet_type)
	    	{
	      	case PACKET_AUTH_RESPONSE:
			new_session->my_id = ntohl(((packet_auth_response*)(packet))->userid);
			break;
	      	case PACKET_CLIENT_ADDRESS:
			new_session->connect_addr.sin_port = ((packet_client_address*)(packet))->port;
			new_session->connect_addr.sin_addr.s_addr = ((packet_client_address*)(packet))->address;
	       		connect_to_client(new_session);
			break;
		case PACKET_CHAT_MESSAGE:
			printf("%d : %s\n", ((packet_chat_message*)packet)->senderid, ((packet_chat_message*)packet)->message.text);	      	
		default:
			break;
	    		}
	}
}

void session_client(void *arg)
{
  	//char buf[512];
 	char *command_t = malloc(1);
 	int server_socket = new_session->s_serv;
	while(1){
	scanf("%s", command_t);
	to_upper(command_t);
	printf("%s\n", command_t);
      	if(strcmp(command_t, "ID") == 0){
	        printf("My id = %d", new_session->my_id);
	        fflush(stdout);
	}
	if(strcmp(command_t, "HOST") == 0)
		command_host(server_socket);
	if(strcmp(command_t, "MESSAGE") == 0)
		command_message(server_socket);
	if(strcmp(command_t, "COMMANDS") == 0)
	        printf("id\nadd        <id>\nmessage     <id><text>\nhost       <port> <id>\n");
	}  
}

