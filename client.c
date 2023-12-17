#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/sha.h>
#include "server.h"
#define PORT 12345
#define MAX_BUFFER_SIZE 4096

void check(int code)
{
    if (code == -1)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }
}

void login(int client_socket)
{
    Message message;
    message.type = LOGIN; // or REGISTER
    char username[50];
    char password[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    strcpy(message.data.loginData.username, username);
    strcpy(message.data.loginData.password, password);
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }

    // // Receive response from server
    Response response;
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
}

void sendRegister(int client_socket)
{
    Message message;
    message.type = REGISTER; // or REGISTER
    char username[50];
    char password[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    strcpy(message.data.registerData.username, username);
    strcpy(message.data.registerData.password, password);
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }

    // // Receive response from server
    Response response;
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
    case REGISTER_RESPONSE:
        if (response.data.registerResponse.is_success == 1)
        {
            printf("Register success\n");
        }
        else
        {
            printf("Register failed\n");
        }
        break;

    default:
        printf("Invalid response\n");
        break;
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
    while (1)
    {
        int choice;
        printf("TEST CLIENT\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            login(client_socket);
            break;
        case 2:
            sendRegister(client_socket);
            break;
        case 3:
            close(client_socket);
            exit(EXIT_SUCCESS);
            break;
        default:
            printf("Invalid choice\n");
            break;
        }
    }
}