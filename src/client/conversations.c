#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

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

void direct_packet_send()
{
  char buf[512];
  char *command_t = malloc(1);
  while(1)
  {
	scanf("%s", command_t);
	to_upper(command_t);
	if(strcmp(command_t, "MESSAGE"))
	{
	gets(buf);
	//packet_send(user->socket, LOGIN, strlen(user->buf)+1, user->buf);
    }
  }
}
void direct_packet_recv(int socket)
{
  packet_type_t packet_type;
  packet_length_t length;
  void *packet;
  pthread_t thread;

  if((pthread_create(&thread, NULL, (void*)direct_packet_send, NULL)) != 0 )
  {
    perror("pthread: ");
    exit(1);  
  }

  while(1)
  {
    if( !packet_recv(socket, &packet_type, &length, &packet)) 
    {
      printf("Disconnect to server\n");
      exit(1);
    }
    packet_debug_full(packet_type, length, packet);
    switch(packet_type)
    {
      case 0:
	break;
      default:
	break;
    }
  }  
  
}

void hosting(int port, int userid)
{
  conversation conv;
  struct sockaddr_in addr;
  socklen_t addrlen;

  if ((conv.socket = socket(AF_INET, SOCK_STREAM,0)) == -1)
  {
    perror("socket(): ");
    exit(2);
  }

  conv.port = htons((u_short)port);
  addr.sin_family = AF_INET;			
  addr.sin_port = conv.port;
  
  if (bind(conv.socket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
  {
    perror("bind(): ");
    exit(2);
  }

  if (listen(conv.socket, 1) == -1)
  {
    perror("listen(): ");
    exit(2);
  }

  while((conv.socket_client = accept(conv.socket, (struct sockaddr*)&addr, &addrlen)) == -1)
  {   
    direct_packet_recv(conv.socket_client);
  }
}

void direct_connection(struct sockaddr_in addr)
{ 
  int s;

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Client: socket(): ");
    exit(2);
  }

  if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1){
    perror("Client: connect(): ");
    exit(2);
  }
  direct_packet_recv(s);
}
 
