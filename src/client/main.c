#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <libgen.h>
#include <dirent.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>
#include "../shared/networking.h"

typedef struct cl
{
  int s_serv;
  int s_cl;
  int online;
  void **packet;
  char *buf;
  char *command_t;
  int my_s_id;
  uint32_t my_id;
  packet_type_t packet_type;
  packet_length_t length;
  struct hostent *hp;
  struct sockaddr_in serv_addr;
  pthread_t thread;
}cl;

void to_upper(char *s)
{
  int i;
  char q;
    for(i = 0;i < strlen(s); i++)
    {
      q = *(s+i);
      *(s+i) = toupper(q);
    }
}

void session_client(cl *arg)
{
  packet_auth_request auth;
  cl user = *arg;
  user.command_t = malloc(1);
  while(1)
  {
    printf("ololo\n");
    fflush(stdout);
    scanf("%s", user.command_t);
    to_upper(user.command_t);
    
    if(user.online == 0)
    {
      if(strcmp(user.command_t, "INIT") == 0)
      {
	printf("[Client] user name: \n");
	scanf("%s", auth.login);
	printf("[Client] password: \n");
	scanf("%s", auth.passw);
	packet_send(user.s_serv, PACKET_AUTH_REQUEST, sizeof(packet_auth_request), &auth);
      }
    }
    else
    {
      if(strcmp(user.command_t, "ADD"))
      {
	scanf("%s", user.buf);
	//packet_send(user->socket, LOGIN, strlen(user->buf)+1, user->buf);
      }
      if(strcmp(user.command_t, "LIST"))
      {
	//packet_send(user->socket, LIST, strlen(user->buf)+1, user->buf);
      }
      if(strcmp(user.command_t, "START_CONV"))
      {
	//packet_send(user->socket, LOGIN, strlen(user->buf)+1, user->buf);
      }
      if(strcmp(user.command_t, "COMMAND"))
      {
	printf("init\nadd        <id>\nlist\nstart_conv <id>\n");
      } 
    }
  }
}


void recving(cl user)
{
  while(1)
  {
    if( !packet_recv(user.s_serv, &user.packet_type ,&user.length, user.packet)) 
    {
      perror("Fail packet:");
      exit(1);
    }
    switch(user.packet_type)
    {
      case PACKET_AUTH_RESPONSE:
	user.my_id = ntohl(((packet_auth_response*)(user.packet))->userid);
	break;
      default:
	break;
    }
  }
}
  
int main(int argc, char **argv)
{
  cl user;

  user.online = 0;

  if(argc != 3){
    perror("argc :");
    exit(2);
  }
  
  if ((user.hp = gethostbyname(argv[1])) == 0){
    perror("cl: gethostbyname(): ");
    exit(2);
  }
    
  bzero(&user.serv_addr, sizeof(user.serv_addr));
  bcopy(user.hp->h_addr, &user.serv_addr.sin_addr, user.hp->h_length);
  user.serv_addr.sin_family = user.hp->h_addrtype;
  user.serv_addr.sin_port = htons(atoi(argv[2]));
  
  if ((user.s_serv = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Client: socket(): ");
    exit(2);
  }
// Устанавливаем соединение с сокетом сервера
  if (connect(user.s_serv, (struct sockaddr*)&user.serv_addr, 
	  sizeof(user.serv_addr)) == -1){
    perror("Client: connect(): ");
    exit(2);
  }
  
  //user.name = getenv("USERNAME");
  if((pthread_create(&user.thread, NULL, (void*)session_client, &user)) != 0 )
  {
    perror("pthread: ");
    exit(1);  
   }
   
  recving(user);
  close(user.s_serv);
  printf("Client: Client exited.\n");

  return 0;
}


