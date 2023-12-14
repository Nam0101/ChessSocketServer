#include "sqlite3.h"
#include <stdio.h>

int main()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    // delete table
    char *sql1 = "DROP TABLE user;";
    rc = sqlite3_exec(db, sql1, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    char *sql = "CREATE TABLE IF NOT EXISTS user ("
                "userId INTEGER PRIMARY KEY AUTOINCREMENT,"
                "username TEXT NOT NULL,"
                "password TEXT NOT NULL,"
                "elo INTEGER NOT NULL);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    // insert data
    char *sql2 = "INSERT INTO user (username, password, elo) VALUES ('Nam', '123', 0);";
    rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
    return 0;
}