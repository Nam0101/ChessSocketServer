// user.c
#include <stdio.h>
#include <sys/socket.h>
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
        close_database_connection(db);
        return NULL;
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
        const unsigned char *usernameConst = sqlite3_column_text(stmt, 1);
        size_t usernameLength = strlen((const char *)usernameConst);

        // Malloc và sao chép dữ liệu từ usernameConst vào username
        char *username = (char *)malloc(usernameLength + 1);
        strcpy(username, (const char *)usernameConst);

        const unsigned char *passwordConst = sqlite3_column_text(stmt, 2);
        size_t passwordLength = strlen((const char *)passwordConst);

        // Malloc và sao chép dữ liệu từ passwordConst vào password
        char *password = (char *)malloc(passwordLength + 1);
        strcpy(password, (const char *)passwordConst);

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
    Response *response = (Response *)malloc(sizeof(Response));
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
    int bytes_sent = send(client_socket, response, sizeof(Response), 0);
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

    sqlite3 *db = get_database_connection();

    if (db == NULL)
    {
        handle_register_error(client_socket, "Cannot open database");
        return;
    }

    if (username_exists(db, username))
    {
        handle_register_error(client_socket, "Username exists");
        close_database_connection(db);
        free(username);
        free(password);
        free(hashed_password);
        return;
    }

    if (register_user(db, username, hashed_password))
    {
        printf("Register success\n");
    }
    else
    {
        printf("Register failed\n");
    }

    close_database_connection(db);
    free(username);
    free(password);
    free(hashed_password);
}

void handle_register_error(const int client_socket, const char *message)
{
    printf("%s\n", message);

    Response *response = (Response *)malloc(sizeof(Response));
    response->type = REGISTER_RESPONSE;
    response->data.registerResponse.is_success = 0;
    strncpy(response->data.registerResponse.message, message, sizeof(response->data.registerResponse.message));

    int bytes_sent = send(client_socket, response, sizeof(Response), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }

    free(response);
}

int username_exists(sqlite3 *db, const char *username)
{
    char *sql_is_exist = "SELECT * FROM user WHERE username = ?;";
    sqlite3_stmt *stmt_is_exist;
    int rc = sqlite3_prepare_v2(db, sql_is_exist, -1, &stmt_is_exist, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt_is_exist, 1, username, strlen(username), 0);

    rc = sqlite3_step(stmt_is_exist);
    sqlite3_finalize(stmt_is_exist);

    return (rc == SQLITE_ROW);
}

int register_user(sqlite3 *db, const char *username, const char *hashed_password)
{
    char *sql = "INSERT INTO user (username, password, elo) VALUES (?, ?, 1000);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    sqlite3_bind_text(stmt, 2, hashed_password, strlen(hashed_password), 0);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}
