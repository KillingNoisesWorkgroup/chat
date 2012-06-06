#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#include "session.h"
#include "conversations.h"
#include "../shared/networking.h"

void pr(char *str){
	printf("%s[%s]", str, new_session->login);
	fflush(stdout);
}

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
		pr("incorrectly entered : <address>:<port> [/connect 0.0.0.0:1111]\n");	
	}else{
		connecting(str);
  	}
}

void command_host(char *strline){
	uint32_t port;
	char *trash = malloc(1);
	if(sscanf(strline, "%s %d",trash, &port) != 2){
		pr("incorrectly entered : <port> [/host 1111]\n");	
	}else{
		new_session->port = htons(port);
		hosting();
  	}
}

void command_p_list(){
	packet_users_list_request packet;
	packet.online_only = 1;	
	packet_send(new_session->s_serv, PACKET_USERS_LIST_REQUEST, sizeof(packet_users_list_request), &packet);
}

void command_p_olist(){
	packet_users_list_request packet;
	packet.online_only = 0;	
	packet_send(new_session->s_serv, PACKET_USERS_LIST_REQUEST, sizeof(packet_users_list_request), &packet);
}

void command_message(char* message, uint16_t userid){
	packet_chat_message packet;
	packet.senderid = new_session->my_id;
	packet.receiverid = userid;
	memcpy(packet.message.text, parse_message(message), CHAT_MESSAGE_MAXSIZE);
	if(userid == new_session->host_id){
		packet_send(new_session->s_cl, PACKET_CHAT_MESSAGE, sizeof(packet_chat_message), &packet);	
	}else
	{
		packet_send(new_session->s_serv, PACKET_CHAT_MESSAGE, sizeof(packet_chat_message), &packet);
	}
}


void create_session(char *login, char *passw, int server_socket,struct sockaddr_in *serv_addr){
	packet_auth_request auth;
	if((new_session = malloc(sizeof(session))) == NULL){
		perror("malloc");
		exit(1);	
	}	
	strcpy(new_session->login, login);
	new_session->host_id = -1;
	new_session->s_serv = server_socket;
  	new_session->addr = *serv_addr;
	
	strcpy(auth.login, login);
	auth.login[-1] = '\0';
	MD5((unsigned char *) passw, strlen(passw), (unsigned char*)&auth.passw);
  	packet_send(new_session->s_serv, PACKET_AUTH_REQUEST, sizeof(packet_auth_request), &auth);
  	pr("");
 	if((pthread_create(&new_session->thread, NULL, (void*)session_client, NULL)) != 0 )
  	{
    	perror("pthread: ");
    	exit(1);  
  	}
	recving_packets(new_session->s_serv);
}

void session_client(void *arg)
{
 	char *command_t = malloc(1);
	char *strline = malloc(1);	
	size_t str_len;
	pr("Connecting to server\n");
	uint16_t userid;
	while(1){
		getline(&strline, &str_len, stdin);
		pr("");		
		if((userid = parse_id(strline)) != 0){
			command_message(strline, userid);
			continue;
		}
		sscanf(strline, "%s", command_t);
		to_upper(command_t);
		if(strcmp(command_t, "/HOST") == 0){
			command_host(strline);
			continue;	
		}
		if(strcmp(command_t, "/CONNECT") == 0){
			command_connect(strline);
			continue;
		}	
		if(strcmp(command_t, "/P_LIST") == 0){
			command_p_list();
			continue;
		}
		if(strcmp(command_t, "/P_OLIST") == 0){
			command_p_olist();
			continue;
		}
		if(strcmp(command_t, "/COMMANDS") == 0){
			pr("/p_list\n");
			pr("/p_olist\n");
			pr("/host       <port>\n");
			pr("/connect    <address>:<port>\n");
			continue;
		}
		pr("Unknown command %s. Try use /commands");
	}
	free(command_t);
	free(strline);
}

