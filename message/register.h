#ifndef REGISTER_H
#define REGISTER_H
typedef struct
{
    char username[20];
    char password[10];
} RegisterData;
typedef struct
{
    short is_success;
    char message_code;
} RegisterResponse;
#endif