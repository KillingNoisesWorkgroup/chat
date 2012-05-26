#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

#include "session.h"
#include "../shared/networking.h"

void create_session(char *login, char *passw, int server_socket,struct sockaddr_in *serv_addr)
{
  packet_auth_request auth;
  packet_type_t packet_type;
  packet_length_t length;
  void *packet;
  session new_session;
  
  strcpy(auth.login, login);
  auth.login[PLAYER_NAME_MAXSIZE-1] = '\0';
  MD5((unsigned char *) passw, strlen(passw), (unsigned char*)&auth.passw);
  packet_send(server_socket, PACKET_AUTH_REQUEST, sizeof(packet_auth_request), &auth);
  
  if((pthread_create(&new_session.thread, NULL, (void*)session_client, NULL)) != 0 )
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
      default:
	break;
    }
  }

}
  

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

void session_client(void *arg)
{
  char buf[512];
  char *command_t = malloc(1);
  while(1)
  {
    scanf("%s", command_t);
    to_upper(command_t);
    if(strcmp(command_t, "ADD"))
    {
      scanf("%s", buf);
      //packet_send(user->socket, LOGIN, strlen(user->buf)+1, user->buf);
    }
    if(strcmp(command_t, "LIST"))
    {
      //packet_send(user->socket, LIST, strlen(user->buf)+1, user->buf);
    }
    if(strcmp(command_t, "START_CONV"))
    {
      //packet_send(user->socket, LOGIN, strlen(user->buf)+1, user->buf);
    }
    if(strcmp(command_t, "COMMAND"))
    {
      printf("init\nadd        <id>\nlist\nstart_conv <id>\n");
    } 
  }  
}

