// user.c
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "sqlite3.h"
#include <pthread.h>
#include "user.h"
#include "../database/database.h"
#define SERVER_ERROR 'E'
#define USERNAME_EXISTS 'U'
#define REGISTER_SUCCESS 'S'
#define USER_LOGED_IN 'L'
#define USERNAME_PASSWORD_WRONG 'W'
#define ALREADY_FRIEND 'A'
#define FRIEND_ID_NOT_FOUND 'F'
#define LOGIN_QUERY "SELECT * FROM user WHERE username = ? AND password = ?;"
#define CHECK_USERNAME_QUERY "SELECT * FROM user WHERE username = ?;"
#define INSERT_USER_QUERY "INSERT INTO user (username, password, elo) VALUES (?, ?, 1000);"
#define ADD_FRIEND_QUERY "INSERT INTO friend (user_id, friend_id) VALUES (?, ?);"
#define GET_FRIEND_LIST_QUERY "SELECT * FROM friend WHERE user_id = ?;"
#define CHECK_ALREADY_FRIEND_QUERY "SELECT * FROM friend WHERE user_id = ? AND friend_id = ?;"
#define CHECK_USER_EXIST_BY_ID_QUERY "SELECT * FROM user WHERE id = ?;"
loged_in_user_t *online_user_list = NULL;
pthread_mutex_t online_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_online_user(int user_id, int elo, int client_socket, char *username)
{
    loged_in_user_t *new_user = (loged_in_user_t *)malloc(sizeof(loged_in_user_t));
    new_user->user_id = user_id;
    new_user->elo = elo;
    new_user->is_playing = 0;
    new_user->is_finding = 0;
    strcpy(new_user->username, username);
    new_user->client_socket = client_socket;
    new_user->next = online_user_list;
    online_user_list = new_user;
}

void remove_online_user(int user_id)
{
    pthread_mutex_lock(&online_list_mutex);
    loged_in_user_t *current = online_user_list;
    loged_in_user_t *previous = NULL;
    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            if (previous == NULL)
            {
                online_user_list = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            break;
        }
        previous = current;
        current = current->next;
    }
    pthread_mutex_unlock(&online_list_mutex);
}

loged_in_user_t *get_list_online_user()
{
    return online_user_list;
}

void hash_password(const char *password, char *hashed_password)
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Final((unsigned char *)hashed_password, &sha256);
}

user_t *login(const char *username, const char *password)
{
    // open database
    sqlite3 *db = get_database_connection();
    // create query
    char *sql = LOGIN_QUERY;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        close_database_connection(db);
        return NULL;
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
        const unsigned char *usernameConst = sqlite3_column_text(stmt, 1);
        size_t usernameLength = strlen((const char *)usernameConst);

        // Malloc và sao chép dữ liệu từ usernameConst vào username
        char *username = (char *)malloc(usernameLength + 1);
        strcpy(username, (const char *)usernameConst);

        const unsigned char *passwordConst = sqlite3_column_text(stmt, 2);
        size_t passwordLength = strlen((const char *)passwordConst);

        // Malloc và sao chép dữ liệu từ passwordConst vào password
        char *password = (char *)malloc(passwordLength + 1);
        strcpy(password, (const char *)passwordConst);

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
        close_database_connection(db);
        return user;
    }
    sqlite3_finalize(stmt);
    close_database_connection(db);
    return NULL;
}

void print_user(user_t *user)
{
    printf("User id: %d\n", user->user_id);
    printf("Username: %s\n", user->username);
    printf("Elo: %d\n", user->elo);
}

// Kiểm tra xem người dùng đã đăng nhập hay chưa
int isUserAlreadyLoggedIn(const char *username)
{
    loged_in_user_t *current = online_user_list;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            return 1; // Người dùng đã đăng nhập
        }
        current = current->next;
    }
    return 0; // Người dùng chưa đăng nhập
}

// Kiểm tra đăng nhập và trả về thông tin người dùng
user_t *performLogin(const char *username, const char *password)
{
    char *hashed_password = (char *)malloc(65);
    hash_password(password, hashed_password);
    user_t *user = login(username, hashed_password);
    free(hashed_password);
    return user;
}

// Gửi phản hồi đăng nhập cho client
void sendLoginResponse(const int client_socket, int isSuccess, int messageCode, user_t *user)
{
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = LOGIN_RESPONSE;
    response->data.loginResponse.is_success = isSuccess;
    response->data.loginResponse.message_code = messageCode;
    if (user != NULL)
    {
        response->data.loginResponse.user_id = user->user_id;
        response->data.loginResponse.elo = user->elo;
    }

    int bytes_sent = send(client_socket, response, sizeof(Response), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }

    free(response);
}

// Hàm xử lý yêu cầu đăng nhập
void handle_login(const int client_socket, const LoginData *loginData)
{
    if (isUserAlreadyLoggedIn(loginData->username))
    {
        // Người dùng đã đăng nhập
        sendLoginResponse(client_socket, 0, USER_LOGED_IN, NULL);
        return;
    }

    char *username = strdup(loginData->username);
    char *password = strdup(loginData->password);

    user_t *user = performLogin(username, password);
    if (user == NULL)
    {
        // Đăng nhập không thành công
        sendLoginResponse(client_socket, 0, USERNAME_PASSWORD_WRONG, NULL);
    }
    else
    {
        // Đăng nhập thành công, thêm người dùng vào danh sách đang trực tuyến
        pthread_mutex_lock(&online_list_mutex);
        add_online_user(user->user_id, user->elo, client_socket, user->username);
        pthread_mutex_unlock(&online_list_mutex);

        // Gửi phản hồi đăng nhập thành công
        sendLoginResponse(client_socket, 1, 0, user);
    }

    free(username);
    free(password);
    free(user);
}

void handle_register(const int client_socket, const RegisterData *registerData)
{
    char *username = (char *)malloc(20);
    char *password = (char *)malloc(10);
    strcpy(username, registerData->username);
    strcpy(password, registerData->password);
    char *hashed_password = (char *)malloc(65);

    hash_password(password, hashed_password);

    sqlite3 *db = get_database_connection();

    if (db == NULL)
    {
        send_register_message(client_socket, SERVER_ERROR, 0);
        return;
    }

    if (username_exists(db, username))
    {
        send_register_message(client_socket, USERNAME_EXISTS, 0);
        close_database_connection(db);
        free(username);
        free(password);
        free(hashed_password);
        return;
    }

    if (register_user(db, username, hashed_password))
    {
        send_register_message(client_socket, REGISTER_SUCCESS, 1);
    }
    else
    {
        printf("Register failed\n");
    }

    close_database_connection(db);
    free(username);
    free(password);
    free(hashed_password);
}

void send_register_message(const int client_socket, const char message_code, int is_success)
{

    Response *response = (Response *)malloc(sizeof(Response));
    response->type = REGISTER_RESPONSE;
    response->data.registerResponse.is_success = is_success;
    response->data.registerResponse.message_code = message_code;

    int bytes_sent = send(client_socket, response, sizeof(Response), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }

    free(response);
}

int username_exists(sqlite3 *db, const char *username)
{
    char *sql_is_exist = CHECK_USERNAME_QUERY;
    sqlite3_stmt *stmt_is_exist;
    int rc = sqlite3_prepare_v2(db, sql_is_exist, -1, &stmt_is_exist, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt_is_exist, 1, username, strlen(username), 0);

    rc = sqlite3_step(stmt_is_exist);
    sqlite3_finalize(stmt_is_exist);

    return (rc == SQLITE_ROW);
}

int register_user(sqlite3 *db, const char *username, const char *hashed_password)
{
    char *sql = INSERT_USER_QUERY;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, strlen(username), 0);
    sqlite3_bind_text(stmt, 2, hashed_password, strlen(hashed_password), 0);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

int get_friend_list(const int user_id, int *friend_list)
{
    sqlite3 *db = get_database_connection();
    char *sql = GET_FRIEND_LIST_QUERY;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    int i = 0;
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        close_database_connection(db);
        return 0;
    }
    sqlite3_bind_int(stmt, 1, user_id);
    rc = sqlite3_step(stmt);
    while (rc == SQLITE_ROW)
    {
        friend_list[i] = sqlite3_column_int(stmt, 2);
        i++;
        rc = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    close_database_connection(db);
    return i;
}

// Hàm kiểm tra sự tồn tại của người dùng theo ID
int checkUserExistByID(sqlite3 *db, int userId)
{
    sqlite3_stmt *stmt;
    char *sql = CHECK_USER_EXIST_BY_ID_QUERY;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(stmt, 1, userId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_ROW);
}

// Hàm kiểm tra xem hai người dùng đã là bạn bè chưa
int checkAlreadyFriend(sqlite3 *db, int userId, int friendId)
{
    sqlite3_stmt *stmt;
    char *sql = CHECK_ALREADY_FRIEND_QUERY;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, friendId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_ROW);
}

// Hàm thêm bạn bè
int addFriend(sqlite3 *db, int userId, int friendId)
{
    sqlite3_stmt *stmt;
    char *sql = ADD_FRIEND_QUERY;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, friendId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

// Hàm gửi phản hồi
void sendFriendResponse(const int client_socket, int isSuccess, int messageCode)
{
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = ADD_FRIEND_RESPONSE;
    response->data.addFriendResponse.is_success = isSuccess;
    response->data.addFriendResponse.message_code = messageCode;
    int bytes_sent = send(client_socket, response, sizeof(Response), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    free(response);
}

// Hàm xử lý yêu cầu thêm bạn bè
void handle_add_friend(const int client_socket, const AddFriendData *addFriendData)
{
    sqlite3 *db = get_database_connection();

    if (!checkUserExistByID(db, addFriendData->friend_id))
    {
        sendFriendResponse(client_socket, 0, FRIEND_ID_NOT_FOUND);
        close_database_connection(db);
        return;
    }

    if (checkAlreadyFriend(db, addFriendData->user_id, addFriendData->friend_id))
    {
        sendFriendResponse(client_socket, 0, ALREADY_FRIEND);
        close_database_connection(db);
        return;
    }

    if (!addFriend(db, addFriendData->user_id, addFriendData->friend_id))
    {
        sendFriendResponse(client_socket, 0, SERVER_ERROR);
        close_database_connection(db);
        return;
    }

    // Gửi phản hồi thành công
    sendFriendResponse(client_socket, 1, 0);
    close_database_connection(db);
}
void handle_get_online_friends(const int client_socket, const GetOnlineFriendsData *getOnlineFriendsData)
{
    loged_in_user_t *current = online_user_list;
    int friend_list[100];
    int friend_count = get_friend_list(getOnlineFriendsData->user_id, friend_list);
    int online_friend_count = 0;
    int online_friend_list[100];
    while (current != NULL)
    {
        for (int i = 0; i < friend_count; i++)
        {
            if (current->user_id == friend_list[i])
            {
                online_friend_list[online_friend_count++] = current->user_id;
                break;
            }
        }
        current = current->next;
    }
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = ONLINE_FRIENDS_RESPONSE;
    response->data.onlineFriendsResponse.number_of_friends = online_friend_count;
    for (int i = 0; i < online_friend_count; i++)
    {
        response->data.onlineFriendsResponse.friend_id[i] = online_friend_list[i];
    }
    int bytes_sent = send(client_socket, response, sizeof(Response), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    free(response);
}