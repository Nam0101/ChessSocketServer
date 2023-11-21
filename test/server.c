#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "sqlite3.h"
#include "authentication.pb-c.h"
#include <pthread.h>
#include "node.h"
#define PORT 12345
#define BACKLOG 100
#define MAX_BUFFER_SIZE 4096
#define THREAD_POOL_SIZE 24
int total_clients = 0;

// condition variable
int thread_pool_status[THREAD_POOL_SIZE] = {0};
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct
{
    int client_socket;
    int i;
} ThreadArg;

node_t *head = NULL;

void addTail(int client_socket)
{
    node_t *newNode = (node_t *)malloc(sizeof(node_t));
    newNode->client_socket = client_socket;
    newNode->next = NULL;
    if (head == NULL)
    {
        head = newNode;
    }
    else
    {
        node_t *cur = head;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = newNode;
    }
}

node_t *dequeue()
{
    if (head == NULL)
    {
        return NULL;
    }
    node_t *node = head;
    head = head->next;
    return node;
}

void *handle_client(void *arg)
{
    ThreadArg *thread_arg = (ThreadArg *)arg;
    int client_socket = thread_arg->client_socket;
    free(arg);

    char *buffer = malloc(MAX_BUFFER_SIZE);
    ssize_t num_bytes = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);

    Nam__BaseMessage *base_message = nam__base_message__unpack(NULL, num_bytes, (const uint8_t *)buffer);
    if (!base_message)
    {
        perror("Error unpacking base message");
        close(client_socket);
        pthread_exit(NULL);
    }

    switch (base_message->messagetype)
    {
    case 1:
    {
        Nam__LoginRequest *login_request = base_message->loginrequest;
        if (!login_request)
        {
            perror("Error unpacking login request");
            close(client_socket);
            pthread_exit(NULL);
        }
        // printf("Received login request:\n");
        // printf("Username: %s\n", login_request->username);
        // printf("Password: %s\n", login_request->password);
        break;
    }
    case 2:
    {
        Nam__LogoutRequest *logout_request = base_message->logoutrequest;
        if (!logout_request)
        {
            perror("Error unpacking logout request");
            close(client_socket);
            pthread_exit(NULL);
        }
        // printf("Received logout request:\n");
        // printf("Username: %s\n", logout_request->username);
        break;
    }
    }
    close(client_socket);
    free(buffer);
    pthread_mutex_lock(&mutex);
    thread_pool_status[thread_arg->i] = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
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
    if (listen(server_socket, BACKLOG) == -1)
    {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);
    pthread_t thread_pool[THREAD_POOL_SIZE];

    while (1)
    {
        // Accept connection from client
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        addTail(client_socket);
        if (client_socket == -1)
        {
            perror("Error accepting connection");
            continue;
        }
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Create thread argument
        ThreadArg *thread_arg = (ThreadArg *)malloc(sizeof(ThreadArg));
        if (!thread_arg)
        {
            perror("Error allocating memory for thread argument");
            close(client_socket);
            continue;
        }

        node_t *node = dequeue();
        if (node == NULL)
        {
            printf("Error dequeue\n");
            continue;
        }
        int socket = node->client_socket;
        thread_arg->client_socket = socket;
        free(node);

        int i;
        for (i = 0; i < THREAD_POOL_SIZE; ++i)
        {
            if (thread_pool_status[i] == 0)
            {
                thread_pool_status[i] = 1;
                thread_arg->i = i;
                if (pthread_create(&thread_pool[i], NULL, handle_client, (void *)thread_arg) == 0)
                {
                    printf("Client %d is set to threads %d\n", client_socket, i);
                    total_clients++; // Move this line here
                    printf("Total clients: %d\n", total_clients);
                }
                break;
            }
        }

        if (i == THREAD_POOL_SIZE)
        {
            printf("Thread pool is full. Waiting for a thread to become available...\n");
            pthread_mutex_lock(&mutex);
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
            // Try to find an available thread again
            i = 0;
            continue;
        }
    }

    close(server_socket);

    return 0;
}