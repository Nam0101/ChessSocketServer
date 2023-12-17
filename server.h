#ifndef SERVER_H
#define SERVER_H

#define PORT 12345
#define BACKLOG 100
#define MAX_BUFFER_SIZE 4096
#define THREAD_POOL_SIZE 8

typedef struct
{
    char username[20];
    char password[10];
} LoginData;

typedef struct
{
    char username[20];
    char password[10];
} RegisterData;
typedef struct
{
    int user_id;
} ExitData;
typedef struct
{
    int user_id;
    int friend_id;
} AddFriendData;
typedef struct
{
    short is_success;
    int user_id;
    int elo;
    char message_code;
} LoginResponse;
typedef struct
{
    short is_success;
    char message_code;
} RegisterResponse;
typedef struct
{
    short is_success;
    char message_code;
} AddFriendResponse;
typedef enum
{
    LOGIN,
    REGISTER,
    LOGIN_RESPONSE,
    REGISTER_RESPONSE,
    EXIT,
    ADD_FRIEND,
    ADD_FRIEND_RESPONSE
} MessageType;

typedef struct
{
    MessageType type;
    union
    {
        LoginData loginData;
        RegisterData registerData;
        ExitData exitData;
        AddFriendData addFriendData;
    } data;
} Message;
typedef struct
{
    MessageType type;
    union
    {
        LoginResponse loginResponse;
        RegisterResponse registerResponse;
        AddFriendResponse addFriendResponse;
    } data;
} Response;

#endif
