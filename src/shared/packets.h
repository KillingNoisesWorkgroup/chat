// All data in this file is packed for lowest space
// Do NOT include this file manually! Include networking.h instead!

#define MAX_PACKET_STRING_LENGTH 256
#define PACKET_GENERAL_STRING 100

/* PACKET C2S - Auth Request */

#define USER_NAME_MAXSIZE 256
#define ENCRYPTED_PASSWORD_LENGTH MD5_DIGEST_LENGTH

#define PACKET_AUTH_REQUEST 1
typedef struct packet_auth_request{
	char login[USER_NAME_MAXSIZE];
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


/* PACKET - Message */

#define CHAT_MESSAGE_MAXSIZE 512
typedef struct message_t{
	char text[CHAT_MESSAGE_MAXSIZE];
} message_t;

#define PACKET_CHAT_MESSAGE 6
typedef struct packet_chat_message{
	uint32_t senderid;
	uint32_t receiverid;
	message_t message;
} packet_chat_message;


/* PACKET C2S - Users list request */

#define PACKET_USERS_LIST_REQUEST 7
typedef struct packet_users_list_request{
	uint8_t online_only;
} packet_users_list_request;

#define PACKET_ID_HOST 8
typedef struct packet_id_host{
	uint32_t hostid;
} packet_id_host;


