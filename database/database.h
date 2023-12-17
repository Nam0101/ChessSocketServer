#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#define DATABASE_NAME "database/database.db"

// Function to open the database connection
sqlite3 *get_database_connection();

// Function to close the database connection
void close_database_connection(sqlite3 *db);

#endif /* DATABASE_H */
