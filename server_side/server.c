#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "node.h"
#include "sqlite3.h"
#include "message.pb-c.h"
#include "user.h"
#include <protobuf-c/protobuf-c.h>
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
void check(int code)
{
    if (code == -1)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }
}
user_t *create_user(int user_id, char *username, char *password, char *email, int elo)
{
    user_t *user = malloc(sizeof(user_t));
    user->user_id = user_id;
    user->username = malloc(strlen(username) + 1);
    strcpy(user->username, username);
    user->password = malloc(strlen(password) + 1);
    strcpy(user->password, password);
    user->email = malloc(strlen(email) + 1);
    strcpy(user->email, email);
    user->elo = elo;
    return user;
}
user_t *login(char *username, char *password)
{
    // open database
    sqlite3 *db;
    char *err_msg = 0;
    // mutex lock
    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
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
        char *email = sqlite3_column_text(stmt, 3);
        int elo = sqlite3_column_int(stmt, 4);
        // create user
        user_t *user = create_user(user_id, username, password, email, elo);
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
    printf("Email: %s\n", user->email);
    printf("Elo: %d\n", user->elo);
}

void handle_login(const int client_socket, const Chess__Message *message)
{
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

    user_t *user = login(username, password);
    // create response
    Chess__Message response = CHESS__MESSAGE__INIT;
    response.type = CHESS__MESSAGE__MESSAGE_TYPE__LOGINRESPONSE;
    Chess__LoginResponse login_response = CHESS__LOGIN_RESPONSE__INIT;
    if (user == NULL)
    {
        login_response.success = 0;
    }
    else
    {
        login_response.success = 1;
        login_response.user_id = user->user_id;
        login_response.username = user->username;
        login_response.email = user->email;
        login_response.elo = user->elo;
    }
    response.login_response = &login_response;
    // serialize response
    int response_size = chess__message__get_packed_size(&response);
    if (response_size <= 0)
    {
        perror("Error getting packed size");
        return;
    }
    uint8_t *response_buffer = malloc(response_size);
    if (!response_buffer)
    {
        perror("Error allocating memory");
        return;
    }
    if (chess__message__pack(&response, response_buffer) != response_size)
    {
        perror("Error packing message");
        free(response_buffer);
        return;
    }
    // send response
    int bytes_sent = send(client_socket, response_buffer, response_size, 0);
    if (bytes_sent == -1)
    {
        perror("Error sending message");
        free(response_buffer);
        return;
    }
    printf("Sent %d-byte message\n", bytes_sent);
}

void handle_client(int client_socket)
{
    uint8_t buffer[MAX_BUFFER_SIZE]; // Change this line
    total_clients++;
    int message_size;
    // read message
    message_size = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
    if (message_size == -1)
    {
        perror("Error reading message");
        return;
    }
    // deserialize message
    Chess__Message *message = chess__message__unpack(NULL, message_size, buffer);
    if (message == NULL)
    {
        fprintf(stderr, "Error deserializing message\n");
        return;
    }
    // handle message
    switch (message->type)
    {
    case CHESS__MESSAGE__MESSAGE_TYPE__LOGIN:
        handle_login(client_socket, message);
        break;
    default:
        break;
    }
    printf("Total clients: %d\n", total_clients);
    close(client_socket);
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