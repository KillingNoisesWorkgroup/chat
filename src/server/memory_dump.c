#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "login_entry.h"
#include "database.h"

void read_logins_dump(FILE *logins){
	uint32_t count, login_length, passw_length, id;
	int i;
	login_entry *new_login;
	fread(&count, sizeof(count), 1, logins);
	fread(&last_login_id, sizeof(last_login_id), 1, logins);
	for(i = 0; i < count; i++){
		fread(&id, sizeof(id), 1, logins);
		new_login = init_login_entry(id);
		fread(&login_length, sizeof(login_length), 1, logins);
		fread(new_login->login, login_length, 1, logins);
		new_login->login[login_length] = 0;
		fread(&passw_length, sizeof(passw_length), 1, logins);
		fread(new_login->passw, passw_length, 1, logins);
		dynamic_array_add(database.users, new_login);
	}
}

void read_memory_dump(){
	FILE *logins;
	if( (logins = fopen("logins.dump", "r")) != NULL){
		read_logins_dump(logins);
		fclose(logins);
	}
}

void create_logins_dump(FILE *logins){
	uint32_t count, login_length, passw_length;
	int i;
	login_entry *login;
	count = database.users->size;
	fwrite(&count, sizeof(count), 1, logins);
	fwrite(&last_login_id, sizeof(last_login_id), 1, logins);
	for(i = 0; i < count; i++){
		login = (login_entry*)(database.users->data)[i];
		fwrite(&login->id, sizeof(login->id), 1, logins);
		login_length = strlen(login->login);
		fwrite(&login_length, sizeof(login_length), 1, logins);
		fwrite(login->login, login_length, 1, logins);
		passw_length = strlen(login->passw);
		fwrite(&passw_length, sizeof(passw_length), 1, logins);
		fwrite(login->passw, passw_length, 1, logins);
	}
}

void create_memory_dump(){
	FILE *logins;
	if( (logins = fopen("logins.dump", "w+")) == NULL){
		perror("fopen");
		exit(1);
	}
	create_logins_dump(logins);
	fclose(logins);
}

void* DumpsThread(void* arg){
	int dumps_creation_frequency;
	dumps_creation_frequency = (int)arg;
	while(1){
		sleep(dumps_creation_frequency);
		print_log("dump creation thread", "Started dump creation");
		create_memory_dump();
		print_log("dump creation thread", "Finished dump creation");
	}
}

void create_dump_thread(int dump_creation_frequency){
	int tmp;
	pthread_t thread;
	tmp = dump_creation_frequency;
	if( (pthread_create(&thread, NULL, (void*)DumpsThread, (void*)tmp)) != 0){
		perror("pthread_create");
		exit(1);
	}
}
