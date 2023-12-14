// user.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "sqlite3.h"
#include "user.h"
#include "database.h"
int total_sucess = 0;



void hash_password(const char *password, char *hashed_password)
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Final((unsigned char *)hashed_password, &sha256);
}

user_t *login(char *username, char *password)
{
    // open database
    sqlite3 *db = get_database_connection();
    // mutex lock
    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }
    // create query
    char *sql = "SELECT * FROM user WHERE username = ? AND password = ?;";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // bind parameters
    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    sqlite3_bind_text(stmt, 2, password, strlen(password), 0);
    // execute query
    rc = sqlite3_step(stmt);
    // check result
    if (rc == SQLITE_ROW)
    {
        // get user data
        int user_id = sqlite3_column_int(stmt, 0);
        char *username = sqlite3_column_text(stmt, 1);
        char *password = sqlite3_column_text(stmt, 2);
        int elo = sqlite3_column_int(stmt, 3);
        user_t *user = (user_t *)malloc(sizeof(user_t));
        user->user_id = user_id;
        user->username = (char *)malloc(strlen(username) + 1);
        strcpy(user->username, username);
        user->password = (char *)malloc(strlen(password) + 1);
        strcpy(user->password, password);
        user->elo = elo;
        // close statement
        sqlite3_finalize(stmt);
        // close database
        close_database_connection(db);
        return user;
    }
    sqlite3_finalize(stmt);
    close_database_connection(db);
    return NULL;
}

void print_user(user_t *user)
{
    printf("User id: %d\n", user->user_id);
    printf("Username: %s\n", user->username);
    printf("Elo: %d\n", user->elo);
}

void handle_login(const int client_socket, const LoginData *loginData)
{
    printf("Received login message\n");
    // get login message
    char *username = (char *)malloc(20);
    char *password = (char *)malloc(10);
    strcpy(username, loginData->username);
    strcpy(password, loginData->password);
    char *hashed_password = (char *)malloc(65);
    hash_password(password, hashed_password);
    user_t *user = login(username, hashed_password);
    Message *response = (Message *)malloc(sizeof(Message));
    response->type = LOGIN_RESPONSE;
    if (user == NULL)
    {
        response->data.loginResponse.is_success = 0;
        printf("Login failed\n");
    }
    else
    {
        response->data.loginResponse.is_success = 1;
        response->data.loginResponse.user_id = user->user_id;
        response->data.loginResponse.elo = user->elo;
        printf("Login success\n");
        total_sucess++;
        printf("Total clients: %d\n", total_sucess);
    }
    // send response
    int bytes_sent = send(client_socket, response, sizeof(Message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    free(username);
    free(password);
    free(response);
    free(user);
    free(hashed_password);
}

void handle_register(const int client_socket, const RegisterData *registerData)
{
    char *username = (char *)malloc(20);
    char *password = (char *)malloc(10);
    strcpy(username, registerData->username);
    strcpy(password, registerData->password);
    char *hashed_password = (char *)malloc(65);
    hash_password(password, hashed_password);
    // open database
    sqlite3 *db;
    // mutex lock
    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // create query
    char *sql = "INSERT INTO user (username, password, elo) VALUES (?, ?, 0);";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // bind parameters
    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    sqlite3_bind_text(stmt, 2, hashed_password, strlen(hashed_password), 0);
    // execute query
    rc = sqlite3_step(stmt);
    // check result
    if (rc == SQLITE_DONE)
    {
        printf("Register success\n");
    }
    else
    {
        printf("Register failed\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    free(username);
    free(password);
}
