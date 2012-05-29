#include <stdlib.h>

#include "database.h"
#include "login_entry.h"
#include "memory_dump.h"
#include "session.h"

void create_database(){
	database.users = init_dynamic_array(sizeof(login_entry));
	database.sessions = init_dynamic_array(sizeof(session));
	last_login_id = 1;
	read_memory_dump();
}
