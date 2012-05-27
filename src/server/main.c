#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "database.h"

#define CONNECTION_REQUEST_QUEUE_BACKLOG_SIZE 10

int server_socket;
struct sockaddr_in server_addres;

int main(int argc, char **args){
	int client_socket, port, server, server_reuseaddr, dump_creation_frequency;
	struct sockaddr_in client_addres;
	socklen_t client_socklen;

	if( argc != 3){
		printf("Usage: %s port dump_creation_frequency\n", args[0]);
		printf("Creates a server listening at port and doing memory dumps each dump_creation_frequency seconds\n");
		exit(1);
	}
	
	port = atoi(args[1]);
	dump_creation_frequency = atoi(args[2]);
	
	if( (server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("socket");
		exit(1);
	}
	
	server_reuseaddr = 1;
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &server_reuseaddr, sizeof(server_reuseaddr));
	
	server_addres.sin_family = PF_INET;
	server_addres.sin_port = htons(port);
	server_addres.sin_addr.s_addr = htonl(INADDR_ANY);
	client_socklen = sizeof(struct sockaddr_in);
	
	if( bind(server_socket, (struct sockaddr *)(&server_addres), sizeof(struct sockaddr_in)) == -1 ){
		perror("bind");
		exit(1);
	}
	
	create_database();
	create_dump_thread(dump_creation_frequency);
	
	listen(server_socket, CONNECTION_REQUEST_QUEUE_BACKLOG_SIZE);
	print_log("main", "Waiting for connections on %d port", port);
	
	while(1){
		if( (client_socket = accept(server_socket, (struct sockaddr *)(&client_addres), &client_socklen)) == -1){
			perror("accept");
			exit(1);
		}
		create_session(client_socket, client_addres);
		print_log("main", "Client connected on socket %d", client_socket);
	}
	return 0;
}
