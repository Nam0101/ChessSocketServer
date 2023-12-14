#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/sha.h>
#define PORT 12345
#define MAX_BUFFER_SIZE 4096
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
    short is_success;
    int user_id;
    int elo;
} LoginResponse;

typedef enum
{
    LOGIN,
    REGISTER,
    LOGIN_RESPONSE
} MessageType;

typedef struct
{
    MessageType type;
    union
    {
        LoginData loginData;
        RegisterData registerData;
        LoginResponse loginResponse;
    } data;
} Message;

void check(int code)
{
    if (code == -1)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    int client_socket;
    struct sockaddr_in server_address;

    // Create socket
    check(client_socket = socket(AF_INET, SOCK_STREAM, 0));

    // Connect to server
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    check(connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)));

    // Send login or register request
    Message message;
    message.type = LOGIN; // or REGISTER
    strcpy(message.data.loginData.username, "test");
    strcpy(message.data.loginData.password, "12345678");
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }

    // Receive response from server
    Message response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case LOGIN_RESPONSE:
        if (response.data.loginResponse.is_success == 1)
        {
            printf("Login success\n");
            printf("User id: %d\n", response.data.loginResponse.user_id);
            printf("Elo: %d\n", response.data.loginResponse.elo);
        }
        else
        {
            printf("Login failed\n");
        }
        break;

    default:
        printf("Invalid response\n");
        break;
    }
    // resigter
    // Message message;
    // message.type = REGISTER;
    // strcpy(message.data.registerData.username, "test");
    // strcpy(message.data.registerData.password, "12345678");
    // int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    // if (bytes_sent <= 0)
    // {
    //     printf("Connection closed\n");
    // }
    // else
    // {
    //     printf("Sent: %d bytes\n", bytes_sent);
    // }
    // Receive response from server

    close(client_socket);
    return 0;
}