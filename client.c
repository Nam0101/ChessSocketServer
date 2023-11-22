#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "message.pb-c.h"
#include <protobuf-c/protobuf-c.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    int client_socket;
    struct sockaddr_in server_address;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Specify server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Create message
    Chess__Message message = CHESS__MESSAGE__INIT;
    message.type = CHESS__MESSAGE__MESSAGE_TYPE__LOGIN;

    Chess__LoginMessage login_message = CHESS__LOGIN_MESSAGE__INIT;
    login_message.username = "username";
    login_message.password = "password";
    message.login_message = &login_message;

    int message_size = chess__message__get_packed_size(&message);
    if (message_size <= 0)
    {
        perror("Error getting packed size");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the packed message
    uint8_t *buffer = malloc(message_size);
    if (!buffer)
    {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // Pack the message
    if (chess__message__pack(&message, buffer) != message_size)
    {
        perror("Error packing message");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // Send the packed message
    int bytes_sent = send(client_socket, buffer, message_size, 0);
    if (bytes_sent == -1)
    {
        perror("Error sending message");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    printf("Sent %d-byte message\n", bytes_sent);

    // Free resources
    free(buffer);
    close(client_socket);

    return 0;
}
