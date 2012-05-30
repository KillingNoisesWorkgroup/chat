// All data in this file is packed for lowest space
// Do NOT include this file manually! Include networking.h instead!

#define MAX_PACKET_STRING_LENGTH 256

/* PACKET C2S - Auth Request */

#define PLAYER_NAME_MAXSIZE 256
#define ENCRYPTED_PASSWORD_LENGTH MD5_DIGEST_LENGTH

#define PACKET_AUTH_REQUEST 1
typedef struct packet_auth_request{
	char login[PLAYER_NAME_MAXSIZE];
	char passw[ENCRYPTED_PASSWORD_LENGTH];
} packet_auth_request;


/* PACKET S2C - Auth Response */

#define PACKET_AUTH_RESPONSE 2
typedef struct packet_auth_response{
	uint32_t userid;
} packet_auth_response;


/* PACKET C2S - Direct connection request */

#define PACKET_DIRECT_CONNECTION_REQUEST 3
typedef struct packet_direct_connection_request{
	uint32_t userid;
	uint16_t port;
} packet_direct_connection_request;


/* PACKET S2C - Direct connection client address */

#define PACKET_CLIENT_ADDRESS 4
typedef struct packet_client_address{
	uint16_t port;
	uint32_t address;
} packet_client_address;


/* PACKET C2C - Direct connection close request */

#define PACKET_DIRECT_CONNECTION_CLOSE_REQUEST 5
typedef struct packet_direct_connection_close_request{
	//empty
} packet_direct_connection_close_request;
