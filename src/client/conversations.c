#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "session.h"
#include "conversations.h"
#include "../shared/networking.h"

void to_upper(char *str)
{
  int i;
  char c;
    for(i = 0;i < strlen(str); i++)
    {
      c = *(str+i);
      *(str+i) = toupper(c);
    }
}

void parse_address(char *str, uint16_t *port){
	int i;
	for(i = 0; i < strlen(str);i++){
		if(*(str+i) == ':')break;	
	}
	*port = atoi(str+i+1);
	*(str+i) = '\0'; 
}

void hosting(uint16_t port){
	
	pthread_t host_thread;
	
	if( (pthread_create(&host_thread, NULL, (void *)HostThread, &port)) != 0){
		perror("pthread_create");
		exit(1);
	}
}

void* HostThread(void *arg){
	int host_socket, client_socket;
	struct sockaddr_in host_address, client_address;
	socklen_t client_socklen;

	if( (host_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("socket");
		exit(1);
	}
	
	host_address.sin_family = PF_INET;
	host_address.sin_port = *(uint16_t*)arg;
	host_address.sin_addr.s_addr = htonl(INADDR_ANY);
	client_socklen = sizeof(struct sockaddr_in);
	
	if( bind(host_socket, (struct sockaddr *)(&host_address), sizeof(struct sockaddr_in)) == -1 ){
		perror("bind");
		exit(1);
	}

	listen(host_socket, 10);
	printf("hosting on %s:%d\n", inet_ntoa(host_address.sin_addr), host_address.sin_port);
	if((client_socket = accept(host_socket, (struct sockaddr *)&client_address, &client_socklen)) == -1){
		perror("accept");
		exit(1);
	}
	printf("Connection is n\established");
	recving_packets(client_socket);
	return 0;
}

void connect_to_client(void *arg){
	pthread_t client_connection_thread;
	printf("address before thread creation %s:%d\n", inet_ntoa(new_session->connect_addr.sin_addr), new_session->connect_addr.sin_port);
	if( (pthread_create(&client_connection_thread, NULL, (void *)ClientConnectionThread, NULL)) != 0){
		perror("pthread_create");
		exit(1);
	}
	
	printf("%p\n", new_session);
}

void* ClientConnectionThread(void *arg){
	printf("address after thread creation %s:%d\n", inet_ntoa(new_session->connect_addr.sin_addr), new_session->connect_addr.sin_port);
	if((new_session->s_cl = socket(PF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("socket");
		exit(1);
	}
	if (connect(new_session->s_cl, (struct sockaddr*)&new_session->connect_addr, sizeof(new_session->connect_addr)) == -1){
   		perror("Client: connect(): ");
    		exit(2);
  	}

	printf("%p\n", new_session);
	recving_packets(new_session->s_cl);
	return 0;
}

void connecting(char *str){
	
	pthread_t host_thread;
	if( (pthread_create(&host_thread, NULL, (void *)ConnectionThread, str)) != 0){
		perror("pthread_create");
		exit(1);
	}
}

void* ConnectionThread(void *arg){
	int connection_socket;
	uint16_t port;
	struct hostent *address;

	parse_address((char*)arg, &port);

	printf("Debug parse is %s:%d\n", (char*)arg, port);
	address = gethostbyname((char*)arg);

	bzero(&new_session->connect_addr, sizeof(new_session->connect_addr));	
	bcopy(address->h_addr, &new_session->connect_addr.sin_addr, address->h_length);
	new_session->connect_addr.sin_family = address->h_addrtype;
	new_session->connect_addr.sin_port = htons(port);
	printf("connecting on %s:%d\n", inet_ntoa(new_session->connect_addr.sin_addr), new_session->connect_addr.sin_port);
	if ((connection_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1){
	    perror("Client: socket(): ");
    		exit(2);
  	}

	if (connect(connection_socket, (struct sockaddr*)&new_session->connect_addr, 
		sizeof(new_session->connect_addr)) == -1){
   		perror("Client: connect(): ");
		exit(2);
	}
	printf("connect on %s:%d\n", inet_ntoa(new_session->connect_addr.sin_addr), new_session->connect_addr.sin_port);
	printf("%p\n", new_session);
	recving_packets(connection_socket);
}


void recving_packets(int socket_host){
	packet_type_t packet_type;
	packet_length_t length;
	void *packet;
	while(1){
	    	if( !packet_recv(socket_host, &packet_type, &length, &packet)){
	     		printf("Connection closed\n");
	     		exit(1);
	    	}
		packet_debug_full(packet_type, length, packet);
		switch(packet_type){
	      	case 0:
			break;
	      	default:
			break;
	    	}
	}
}
