// login.h
#ifndef LOGIN_H
#define LOGIN_H

typedef struct
{
    char username[20];
    char password[10];
} LoginData;
typedef struct
{
    short is_success;
    int user_id;
    int elo;
    char message_code;
} LoginResponse;

#endif
