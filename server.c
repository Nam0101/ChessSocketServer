#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "node.h"
#include <pthread.h>
#include "user.h"
#include <semaphore.h>
#include <openssl/sha.h>
#include "server.h"
int total_clients = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t online_cond = PTHREAD_COND_INITIALIZER;
pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_t online_user_list_thread;
pthread_t thread_pool_online_user[THREAD_POOL_SIZE];
queue_t *client_queue;
task_queue_t *task_queue;

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
    Message *message = (Message *)malloc(sizeof(Message));

    check(recv(client_socket, message, MAX_BUFFER_SIZE, 0));
    switch (message->type)
    {
    case LOGIN:
        handle_login(client_socket, &message->data.loginData);
        break;
    case REGISTER:
        handle_register(client_socket, &message->data.registerData);
        break;
    default:
        break;
    }
    free(message);
}

void *thread_function_logedin()
{
    while (1)
    {
        task_t *task;
        pthread_mutex_lock(&pool_mutex);
        while ((task = dequeue_task(task_queue)) == NULL)
        {
            pthread_cond_wait(&online_cond, &pool_mutex);
        }
        pthread_mutex_unlock(&pool_mutex);
        switch (task->message.type)
        {
        case LOGIN:
            handle_login(task->client_socket, &task->message.data.loginData);
            break;
        case REGISTER:
            handle_register(task->client_socket, &task->message.data.registerData);
            break;
        default:
            break;
        }
        free(task);
    }
    pthread_exit(NULL);
}

/// @brief  Listen to online user list, and handle message from online user by select
/// @return NULL
void *listen_online_user_list()
{
    loged_in_user_t *online_user;
    fd_set readfds;
    int max_sd;

    while (1)
    {
        // online_user = get_list_online_user();
        // while (online_user != NULL)
        // {
        //     printf("User %d is online\n", online_user->user_id);
        //     online_user = online_user->next;
        // }

        online_user = get_list_online_user();
        FD_ZERO(&readfds);
        max_sd = 0;

        while (online_user != NULL)
        {
            FD_SET(online_user->client_socket, &readfds);
            if (online_user->client_socket > max_sd)
            {
                max_sd = online_user->client_socket;
            }
            online_user = online_user->next;
        }

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        // printf("Waiting for data...\n");
        int activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        online_user = get_list_online_user();
        while (online_user != NULL)
        {
            if (FD_ISSET(online_user->client_socket, &readfds))
            {
                Message *message = (Message *)malloc(sizeof(Message));
                ssize_t bytes_received = recv(online_user->client_socket, message, MAX_BUFFER_SIZE, 0);
                if (bytes_received <= 0)
                {
                    printf("Connection closed\n");
                    remove_online_user(online_user->user_id);
                }
                // printf("Received message from user %d\n", online_user->user_id);
                pthread_mutex_lock(&pool_mutex);
                enqueue_task(task_queue, online_user->client_socket, *message);
                pthread_cond_signal(&online_cond);
                pthread_mutex_unlock(&pool_mutex);
            }
            printf("Next user\n");
            online_user = online_user->next;
        }
    }
    return NULL;
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
    task_queue = create_task_queue();

    // Create thread pool for handling first message from client (login or register)
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        if (pthread_create(&thread_pool[i], NULL, thread_function, NULL) != 0)
        {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }
    // Create thread for listening online user list
    if (pthread_create(&online_user_list_thread, NULL, listen_online_user_list, NULL) != 0)
    {
        perror("Error creating online user list thread");
        exit(EXIT_FAILURE);
    }
    // Create thread pool for handling message from online user
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        if (pthread_create(&thread_pool_online_user[i], NULL, thread_function_logedin, NULL) != 0)
        {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }
    while (1)
    {
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