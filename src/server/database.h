#ifndef H_SERVER_DATABASE_GUARD
#define H_SERVER_DATABASE_GUARD

#include "dynamic_array.h"

typedef struct database_t{
	dynamic_array *users;
	dynamic_array *sessions;
} database_t;

database_t database;

void create_database();

#endif
