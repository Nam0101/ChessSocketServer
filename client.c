#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "authentication.pb-c.h"

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_BUFFER_SIZE 4096
void send_login_request(int client_socket)
{
    // Create a LoginRequest message
    LoginRequest login_request = LOGIN_REQUEST__INIT;
    login_request.username = "123";
    login_request.password = "123";
    //messagetype is bytes
    login_request.messagetype = (ProtobufCBinaryData){.data = "l", .len = 1};

    // Get the serialized size of the message
    size_t size = login_request__get_packed_size(&login_request);

    // Allocate buffer for serialized message
    uint8_t buffer[MAX_BUFFER_SIZE];

    // Pack the LoginRequest message into the buffer
    login_request__pack(&login_request, buffer);

    // Send the serialized message to the server
    int byte_sent = send(client_socket, buffer, size, 0);
    if (byte_sent == -1)
    {
        perror("Error sending message");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    printf("Sent %d bytes to server\n", byte_sent);
}

int main()
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

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Send a login request to the server
    send_login_request(client_socket);

    // TODO: Receive and process server response here

    // Close the socket
    close(client_socket);

    return 0;
}
