#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#include "session.h"
#include "conversations.h"
#include "../shared/networking.h"

int chat_direct = 0;

void create_session(char *login, char *passw, int server_socket,struct sockaddr_in *serv_addr)
{
  packet_auth_request auth;
  packet_type_t packet_type;
  packet_length_t length;
  struct sockaddr_in addr;
  void *packet;
  session new_session;
  new_session.s_serv = server_socket;
  
  strcpy(auth.login, login);
  auth.login[PLAYER_NAME_MAXSIZE-1] = '\0';
  MD5((unsigned char *) passw, strlen(passw), (unsigned char*)&auth.passw);
  packet_send(server_socket, PACKET_AUTH_REQUEST, sizeof(packet_auth_request), &auth);
  
  if((pthread_create(&new_session.thread, NULL, (void*)session_client, &new_session)) != 0 )
  {
    perror("pthread: ");
    exit(1);  
  }

  while(1)
  {
    if( !packet_recv(server_socket, &packet_type, &length, &packet)) 
    {
      perror("Fail packet:");
      exit(1);
    }
    packet_debug_full(packet_type, length, packet);
    switch(packet_type)
    {
      case PACKET_AUTH_RESPONSE:
	new_session.my_id = ntohl(((packet_auth_response*)(packet))->userid);
	break;
      case PACKET_CLIENT_ADDRESS:
	addr.sin_family = ((packet_client_address*)(packet))->sin_family;
	addr.sin_port = ((packet_client_address*)(packet))->sin_port;
	addr.sin_addr.s_addr = ((packet_client_address*)(packet))->s_addr;
        direct_connection(addr);
	break;
      default:
	break;
    }
  }
}

void session_client(void *new_session)
{
  packet_direct_connection_request conn_user;
  char buf[512];
  char *command_t = malloc(1);
  int server_socket = ((session *)new_session)->s_serv;
  int userid;
  while(1)
  {
      scanf("%s", command_t);
      to_upper(command_t);
	printf("%s\n", command_t);
      if(strcmp(command_t, "ADD") == 0)
      {
        scanf("%s", buf);
        //packet_send(...);
      }
      if(strcmp(command_t, "ID") == 0)
      {
        printf("My id = %d", ((session *)new_session)->my_id);
        fflush(stdout);
      }
      if(strcmp(command_t, "HOST") == 0)
      {
	if(scanf("%s %d", buf, &userid) != 2)
	{
	  printf("Not correct attr\n");	
	}
	else
	{
	  conn_user.userid = htonl((uint32_t)userid);
	  conn_user.port = htons((uint16_t)atoi(buf));
	  packet_send(server_socket, PACKET_DIRECT_CONNECTION_REQUEST, sizeof(packet_direct_connection_request),&conn_user);
	  hosting(atoi(buf), userid);	
  	}
      }
      if(strcmp(command_t, "COMMAND") == 0)
      {
        printf("init\nadd        <id>\nlist\nhost       <port> <id>\n");
      }
   
  }  
}

