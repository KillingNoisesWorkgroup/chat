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
