#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "authentication.pb-c.h"

#define PORT 12345
#define MAX_BUFFER_SIZE 4096

void handle_login(int client_socket)
{
    char buffer[MAX_BUFFER_SIZE];
    ssize_t num_bytes = recv(client_socket, buffer, sizeof(buffer), 0);

    if (num_bytes <= 0)
    {
        perror("Error reading from client");
        close(client_socket);
        return;
    }

    // Parse the LoginRequest
    LoginRequest *login_request = login_request__unpack(NULL, num_bytes, (uint8_t *)buffer);
    if (!login_request)
    {
        fprintf(stderr, "Error unpacking LoginRequest\n");
        close(client_socket);
        return;
    }

    printf("Received LoginRequest:\n");
    printf("Username: %s\n", login_request->username);
    printf("Password: %s\n", login_request->password);

    // TODO: Perform authentication logic here

    // Send a simple response for now
    AuthenticationResponse login_response = AUTHENTICATION_RESPONSE__INIT;
    login_response.success = 1;
    login_response.message = "Login successful";
    //send to client
    int response_len = authentication_response__get_packed_size(&login_response);
    uint8_t *response_buffer = malloc(response_len);
    authentication_response__pack(&login_response, response_buffer);
    send(client_socket, response_buffer, response_len, 0);
    free(response_buffer);
    // Clean up
    login_request__free_unpacked(login_request, NULL);
    close(client_socket);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Error binding");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1)
    {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1)
    {
        // Accept connection from client
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1)
        {
            perror("Error accepting connection");
            continue;
        }

        // Fork a new process to handle the client
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("Error forking");
            close(client_socket);
            continue;
        }
        else if (pid == 0)
        {
            // In child process
            close(server_socket);
            handle_login(client_socket);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // In parent process
            close(client_socket);
        }
    }

    close(server_socket);

    return 0;
}
