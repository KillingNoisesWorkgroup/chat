#ifndef H_CLIENT_SESSION_GUARD
#define H_CLIENT_SESSION_GUARD

#include <pthread.h>
#include <stdint.h>

#include "../shared/networking.h"

typedef struct session
{
  int s_serv;
  int s_cl;
  uint32_t my_id;
  struct sockaddr_in *serv_addr;
  pthread_t thread;
}session;


void create_session(char *login, char *passw, int server_socket,struct sockaddr_in *serv_addr);

void session_client(void *arg);

#endif
