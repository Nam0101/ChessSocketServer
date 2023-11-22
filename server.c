#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "node.h"
#include "sqlite3.h"
#include "message.pb-c.h"
#include <protobuf-c/protobuf-c.h>
#include <pthread.h>
#include <semaphore.h>
#define PORT 12345
#define BACKLOG 100
#define MAX_BUFFER_SIZE 4096
#define THREAD_POOL_SIZE 8
int total_clients = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

queue_t *client_queue;
void check(int code)
{
    if (code == -1)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }
}

void handle_client(int client_socket)
{
    uint8_t buffer[MAX_BUFFER_SIZE]; // Change this line
    int message_size;
    // read message
    message_size = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
    if (message_size == -1)
    {
        perror("Error receiving message");
        return;
    }
    // deserialize message
    Chess__Message *message = chess__message__unpack(NULL, message_size, buffer);
    if (message == NULL)
    {
        perror("Error deserializing message");
        return;
    }
    // handle message
    switch (message->type)
    {
        {
        case CHESS__MESSAGE__MESSAGE_TYPE__LOGIN:
            printf("Received login message\n");
            // get login message
            Chess__LoginMessage *login_message = message->login_message;
            // get username and password
            if (login_message == NULL)
            {
                perror("Error getting login message");
                return;
            }
            char *username = login_message->username;
            char *password = login_message->password;
            printf("Username: %s\n", username);
            printf("Password: %s\n", password);

            break;

        default:
            break;
        }
        printf("Total clients: %d\n", total_clients);
        close(client_socket);
    }
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
        while ((client_socket = dequeue(client_queue)) == -1)
        {
            printf("Thread %ld waiting...\n", pthread_self());
            pthread_cond_wait(&cond, &mutex);
        }
        // unlock mutex
        pthread_mutex_unlock(&mutex);
        // handle client
        handle_client(client_socket);
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
    client_queue = create_queue();

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
        // add client_socket to queue
        enqueue(client_queue, client_socket);

        // unlock mutex
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        // signal condition
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    }

    close(server_socket);

    return 0;
}