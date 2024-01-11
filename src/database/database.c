#include "database.h"
#include <stdio.h>
#include <pthread.h>

#include "../log/log.h"
#define MAX_CONNECTIONS 10
static pthread_mutex_t db_lock = PTHREAD_MUTEX_INITIALIZER;

static sqlite3 *connection_pool[MAX_CONNECTIONS];
static int connection_count = 0;


sqlite3 *get_database_connection()
{
    pthread_mutex_lock(&db_lock);
    if (connection_count > 0)
    {
        // sprintf(msg,"Number of conn to db: %d", connection_count);
        // Log("DB","i",msg);
        sqlite3 *db = connection_pool[--connection_count];
        pthread_mutex_unlock(&db_lock);
        return db;
    }

    sqlite3 *db = NULL;
    if (sqlite3_open(DATABASE_NAME, &db) != SQLITE_OK)
    {
        // sprintf(msg,"Cannot open database: %s\n", sqlite3_errmsg(db));
        // Log("DB","e",msg);
        sqlite3_close(db);
        pthread_mutex_unlock(&db_lock);
        // free(msg); // Move free(msg) here to ensure it's always freed
        return NULL;
    }

    // Enable Write-Ahead Logging (WAL) mode
    char *zErrMsg = 0;
    if (sqlite3_exec(db, "PRAGMA journal_mode=WAL;", 0, 0, &zErrMsg) != SQLITE_OK)
    {
        // sprintf(msg,"Cannot enable WAL mode: %s\n", zErrMsg);
        // Log("DB","e",msg);
        sqlite3_free(zErrMsg);
    }
    pthread_mutex_unlock(&db_lock); // Add unlock here to avoid potential deadlock
    // free(msg); // Add free(msg) here to ensure it's always freed
    return db;
}
// Function to close the database connection
void close_database_connection(sqlite3 *db)
{
    pthread_mutex_lock(&db_lock);

    if (connection_count < MAX_CONNECTIONS)
    {
        connection_pool[connection_count++] = db;
    }
    else
    {
        sqlite3_close(db);
    }

    pthread_mutex_unlock(&db_lock);
}