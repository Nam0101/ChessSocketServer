#ifndef USER_H
#define USER_H
#include <sqlite3.h>
#include "../server.h"
#include <stdio.h>
typedef struct user
{
    int user_id;
    char *username;
    char *password;
    int elo;
} user_t;

typedef struct loged_in_user
{
    int user_id;
    int elo;
    int client_socket;
    char username[20];
    short is_playing;
    short is_finding;
    struct loged_in_user *next;
} loged_in_user_t;

void hash_password(const char *password, char *hashed_password);
user_t *login(const char *username, const char *password);
void print_user(user_t *user);
void handle_login(const int client_socket, const LoginData *loginData);
void handle_register(const int client_socket, const RegisterData *registerData);
void send_register_message(const int client_socket, const char message_code, int is_success);
int username_exists(sqlite3 *db, const char *username);
int register_user(sqlite3 *db, const char *username, const char *hashed_password);
loged_in_user_t *get_list_online_user();
void add_online_user(int user_id, int elo, int client_socket, char *username);
void remove_online_user(int user_id);
void handle_add_friend(const int client_socket, const AddFriendData *addFriendData);
int get_friend_list(const int user_id, FriendDataResponse *friend_list);
int checkUserExistByID(sqlite3 *db, int userId);
int checkAlreadyFriend(sqlite3 *db, int userId, int friendId);
int addFriend(sqlite3 *db, int userId, int friendId);
void sendFriendResponse(const int client_socket, int isSuccess, int messageCode);
int isUserAlreadyLoggedIn(const char *username);
int isUserAlreadyLoggedIn(const char *username);
user_t *performLogin(const char *username, const char *password);
void sendLoginResponse(const int client_socket, int isSuccess, int messageCode, user_t *user);
void handle_get_online_friends(const int client_socket, const GetOnlineFriendsData *getOnlineFriendsData);
char *get_user_name_by_user_id(int user_id);
void get_user_info_by_user_name(const char *username, int *elo, int *user_id, int *is_online, int *is_playing);
#endif