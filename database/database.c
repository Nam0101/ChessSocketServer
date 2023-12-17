#include "database.h"
#include <stdio.h>
#include <pthread.h>
#define MAX_CONNECTIONS 10
static pthread_mutex_t db_lock;

static sqlite3 *connection_pool[MAX_CONNECTIONS];
static int connection_count = 0;
void initialize_database()
{
    pthread_mutex_init(&db_lock, NULL);
}

// Function to open the database connection
sqlite3 *get_database_connection()
{
    pthread_mutex_lock(&db_lock);

    // Check if there's a connection in the pool
    if (connection_count > 0)
    {
        sqlite3 *db = connection_pool[--connection_count];
        pthread_mutex_unlock(&db_lock);
        return db;
    }

    // If the pool is empty, open a new connection
    sqlite3 *db = NULL;
    if (sqlite3_open(DATABASE_NAME, &db) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        pthread_mutex_unlock(&db_lock);
        return NULL;
    }

    pthread_mutex_unlock(&db_lock);
    return db;
}

// Function to close the database connection
void close_database_connection(sqlite3 *db)
{
    pthread_mutex_lock(&db_lock);

    // Check if the pool is not full
    if (connection_count < MAX_CONNECTIONS)
    {
        connection_pool[connection_count++] = db;
    }
    else
    {
        // If the pool is full, close the connection
        sqlite3_close(db);
    }

    pthread_mutex_unlock(&db_lock);
}
