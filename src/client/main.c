#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include <ctype.h>
#include <openssl/md5.h>

#include "session.h"
#include "../shared/networking.h"
  
int main(int argc, char **argv)
{
	int server_socket;
	struct sockaddr_in serv_addr;

	if(argc != 5){
		perror("argc :");
		exit(2);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));

	if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("Client: socket(): ");
		exit(2);
	}

	if (connect(server_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		perror("Client: connect(): ");
		exit(2);
	}
  
	create_session(argv[3], argv[4], server_socket, &serv_addr);
	close(server_socket);
	printf("Client: Client exited.\n");

	return 0;
}


