#ifndef USER_H
#define USER_H

#include <stdio.h>
typedef struct user
{
    int user_id;
    char *username;
    char *password;
    int elo;
} user_t;

user_t *create_user(int user_id, char *username, char *password, int elo);
void destroy_user(user_t *user);
void print_user(user_t *user);
void print_user_list(user_t **user_list, int size);

#endif