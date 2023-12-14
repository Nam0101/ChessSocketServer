#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "node.h"
#include "sqlite3.h"
#include "user.h"
#include <pthread.h>
#include <sodium.h>
#include <semaphore.h>
#define PORT 12345
#define BACKLOG 100
#define MAX_BUFFER_SIZE 4096
#define THREAD_POOL_SIZE 8
int total_clients = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

queue_t *client_queue;

typedef struct
{
    char username[20];
    char password[10];
} LoginData;

typedef struct
{
    char username[20];
    char password[10];
    char name[20];
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

user_t *login(char *username, char *password)
{
    // open database
    sqlite3 *db;
    // mutex lock
    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }
    // create query
    char *sql = "SELECT * FROM user WHERE username = ? AND password = ?;";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    // bind parameters
    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    sqlite3_bind_text(stmt, 2, password, strlen(password), 0);
    // execute query
    rc = sqlite3_step(stmt);
    // check result
    if (rc == SQLITE_ROW)
    {
        // get user data
        int user_id = sqlite3_column_int(stmt, 0);
        char *username = sqlite3_column_text(stmt, 1);
        char *password = sqlite3_column_text(stmt, 2);
        int elo = sqlite3_column_int(stmt, 3);
        user_t *user = (user_t *)malloc(sizeof(user_t));
        user->user_id = user_id;
        user->username = (char *)malloc(strlen(username) + 1);
        strcpy(user->username, username);
        user->password = (char *)malloc(strlen(password) + 1);
        strcpy(user->password, password);
        user->elo = elo;
        // close statement
        sqlite3_finalize(stmt);
        // close database
        sqlite3_close(db);
        return user;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return NULL;
}
void print_user(user_t *user)
{
    printf("User ID: %d\n", user->user_id);
    printf("Username: %s\n", user->username);
    printf("Password: %s\n", user->password);
    printf("Elo: %d\n", user->elo);
}

void handle_login(const int client_socket, const LoginData *loginData)
{
    printf("Received login message\n");
    // get login message
    char *username = (char *)malloc(20);
    char *password = (char *)malloc(10);
    strcpy(username, loginData->username);
    strcpy(password, loginData->password);
    user_t *user = login(username, password);
    Message *response = (Message *)malloc(sizeof(Message));
    response->type = LOGIN_RESPONSE;
    if (user == NULL)
    {
        response->data.loginResponse.is_success = 0;
        printf("Login failed\n");
    }
    else
    {
        response->data.loginResponse.is_success = 1;
        response->data.loginResponse.user_id = user->user_id;
        response->data.loginResponse.elo = user->elo;
        printf("Login success\n");
    }
    // send response
    int bytes_sent = send(client_socket, response, sizeof(Message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    free(username);
    free(password);
    free(response);
    free(user);
}

void handle_client(int client_socket)
{
    Message message;
    int bytes_received = recv(client_socket, &message, MAX_BUFFER_SIZE, 0);
    switch (message.type)
    {
    case LOGIN:
        handle_login(client_socket, &message.data.loginData);
        break;

    default:
        break;
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