#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "node.h"
#include "sqlite3.h"
#include "protos/message.pb-c.h"
#include <pthread.h>
#include <semaphore.h>
#define PORT 12345
#define BACKLOG 100
#define MAX_BUFFER_SIZE 4096
#define THREAD_POOL_SIZE 8

//  condition and mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

node_t *head = NULL;

void check(int code)
{
    if (code == -1)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }
}

void handle_connection(int client_socket)
{
    if (client_socket < 0)
    {
        perror("Invalid socket descriptor");
        return;
    }

    char buffer[MAX_BUFFER_SIZE];
    int message_size;
    // Receive message from client
    check(message_size = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0));
    printf("Message received from client: %s\n", buffer);
}

void *thread_function()
{
    while (1)
    {
        // get client_socket from queue
        // lock mutex
        int client_socket;
        pthread_mutex_lock(&mutex);
        // wait if queue is empty
        while ((client_socket = dequeue(&head)) == -1)
        {
            pthread_cond_wait(&cond, &mutex);
            client_socket = dequeue(&head);
        }
        // unlock mutex
        pthread_mutex_unlock(&mutex);
        // handle connection
        if (client_socket == -1)
        {
            continue;
        }
        // handle connection
        handle_connection(client_socket);
        close(client_socket);
    }
}

int main()
{
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Create socket
    check(server_socket = socket(AF_INET, SOCK_STREAM, 0));

    // Bind socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    check(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)));

    // Listen for incoming connections
    check(listen(server_socket, BACKLOG));

    printf("Server is listening on port %d...\n", PORT);

    // Create thread pool
    pthread_t thread_pool[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        if (pthread_create(&thread_pool[i], NULL, thread_function, NULL) != 0)
        {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }
    while (1)
    {
        // lock mutex
        // Accept connection from client
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        // unlock mutex

        if (client_socket == -1)
        {
            perror("Error accepting connection");
            continue;
        }
        // lock mutex
        pthread_mutex_lock(&mutex);
        enqueue(&head, client_socket);
        // unlock mutex
        pthread_mutex_unlock(&mutex);
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // signal condition
        pthread_cond_signal(&cond);
    }

    close(server_socket);

    return 0;
}