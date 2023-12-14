#ifndef USER_H
#define USER_H
#include "server.h"
#include <stdio.h>
typedef struct user
{
    int user_id;
    char *username;
    char *password;
    int elo;
} user_t;
void hash_password(const char *password, char *hashed_password);
user_t *login(char *username, char *password);
void print_user(user_t *user);
void handle_login(const int client_socket, const LoginData *loginData);
void handle_register(const int client_socket, const RegisterData *registerData);

#endif