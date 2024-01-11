#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
// socket
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "../server.h"
#include "../user/user.h"
#include "../database/database.h"
#include "../log/log.h"
#define DEFAULT_TOTAL_TIME 600
#define MAX_SEARCH_TIME 10
#define ELO_THRESHOLD 100 // elo difference between 2 players
#define CREATE_ROOM "INSERT INTO room (white_user_id, black_user_id, total_time) VALUES (?, ?, ?);"
#define UPDATE_ROOM_START_GAME "UPDATE room SET white_user_id = ?, black_user_id = ?, total_time = ?, start_time = ? WHERE id = ?;"
#define LOG_MOVE_QUERY "INSERT INTO move (room_id, piece_id, from_x, from_y, to_x, to_y) VALUES(?, ?, ?, ?, ?, ?);"
#define UPDATE_ROOM_END_GAME "UPDATE room SET end_time = ? , winer_user = ? WHERE id = ?;"
#define GET_HISTORY_QUERY "SELECT \
                            room.id,\
                            user.username AS opponent_name,\
                            user.id AS opponent_id,\
                            start_time,\
                            end_time,\
                            CASE\
                                WHEN winer_user = ? THEN 1 WHEN winer_user = 0 THEN 2 ELSE 0 END AS game_result\
                            FROM room JOIN user ON((room.black_user_id = user.id OR room.white_user_id = user.id) AND user.id != ?)\
                                                WHERE(black_user_id = ? OR white_user_id = ?);"
#define GET_MOVE_HISTORY_QUERY "SELECT move_id,piece_id,from_x,from_y,to_x,to_y FROM move WHERE room_id = ? ORDER BY move_id;"
#define UPDATE_ELO_BY_USER_ID "UPDATE user SET elo = ? WHERE id = ?;"
#define TAG "GAME"
// mutex
#define MAX_NAME_LENGTH 20
#define MAX_TIME_LENGTH 30
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t update_end_game_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t handle_end_game_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t user_mutex = PTHREAD_MUTEX_INITIALIZER;
// list of room
static room_t *list_room = NULL;
room_t *create_room(int white_user_id, int total_time)
{
    room_t *room = (room_t *)malloc(sizeof(room_t));
    room->white_user_id = white_user_id;
    room->black_user_id = -1;
    room->total_time = total_time;
    room->status = 0;
    room->next = NULL;
    return room;
}

void add_room(room_t *room_list, room_t *room)
{
    pthread_mutex_lock(&mutex);
    room_t *current = room_list;
    if (current == NULL)
    {
        list_room = room;
        pthread_mutex_unlock(&mutex);
        return;
    }
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = room;
    pthread_mutex_unlock(&mutex);
}
room_t *get_list_room()
{
    return list_room;
}
void remove_room(room_t *room_list, int room_id)
{
    pthread_mutex_lock(&mutex);
    room_t *current = room_list;
    if (current == NULL)
    {
        pthread_mutex_unlock(&mutex);
        return;
    }
    if (current->room_id == room_id)
    {
        list_room = current->next;
        free(current);
        pthread_mutex_unlock(&mutex);
        return;
    }
    while (current->next != NULL)
    {
        if (current->next->room_id == room_id)
        {
            room_t *temp = current->next;
            current->next = current->next->next;
            free(temp);
            pthread_mutex_unlock(&mutex);
            return;
        }

        current = current->next;
    }
    pthread_mutex_unlock(&mutex);
}
int create_room_db(int white_user_id, int black_user_id, int total_time)
{
    sqlite3 *db = get_database_connection();
    char *sql = CREATE_ROOM;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_bind_int(stmt, 1, white_user_id);
    sqlite3_bind_int(stmt, 2, black_user_id);
    sqlite3_bind_int(stmt, 3, total_time);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    int last_inserted_id = sqlite3_last_insert_rowid(db);
    close_database_connection(db);

    return last_inserted_id;
}

void handle_create_room(const int client_socket, const CreateRoomData *createRoomData)
{
    room_t *room = create_room(createRoomData->user_id, createRoomData->total_time);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = CREATE_ROOM_RESPONSE;
    if (room != NULL)
    {
        response->data.createRoomResponse.is_success = 1;
        response->data.createRoomResponse.message_code = 0;
        int room_id = create_room_db(room->white_user_id, room->black_user_id, room->total_time);
        room->room_id = room_id;
        room->status = 0;
        add_room(get_list_room(), room);
        response->data.createRoomResponse.room_id = room_id;
        ssize_t send_result = send(client_socket, response, sizeof(Response), 0);
        if (send_result == -1)
        {
            char *log_msg = (char *)malloc(sizeof(char) * 100);
            sprintf(log_msg, "Failed to send response to client socket %d", client_socket);
            Log(TAG, "e", log_msg);
            free(log_msg);
        }
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Created room %d", room_id);
        Log(TAG, "i", log_msg);
        free(log_msg);
    }
    else
    {
        response->data.createRoomResponse.is_success = 0;
        response->data.createRoomResponse.message_code = 1;
        ssize_t send_result = send(client_socket, response, sizeof(Response), 0);
        if (send_result == -1)
        {
            char *log_msg = (char *)malloc(sizeof(char) * 100);
            sprintf(log_msg, "Failed to send response to client socket %d", client_socket);
            Log(TAG, "e", log_msg);
            free(log_msg);
        }
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Failed to create room");
        Log(TAG, "e", log_msg);
        free(log_msg);
        free(room);
    }
    free(response);
}

void send_invite_friend(int friend_socket, int user_id, int room_id, int total_time, char *username)
{
    Response *response;
    response->type = INVITE_FRIEND_RESPONSE;
    response->data.inviteFriendResponse.user_id = user_id;
    response->data.inviteFriendResponse.room_id = room_id;
    response->data.inviteFriendResponse.total_time = total_time;
    strcpy(response->data.inviteFriendResponse.username, username);
    ssize_t send_result = send(friend_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Failed to send response to client socket %d", friend_socket);
        Log(TAG, "e", log_msg);
        free(log_msg);
    }
}
void handle_invite_friend(const int client_socket, const InviteFriendData *inviteFriendData)
{
    int friend_id = inviteFriendData->friend_id;
    int room_id = inviteFriendData->room_id;
    int user_id = inviteFriendData->user_id;
    int total_time = inviteFriendData->total_time;
    int friend_socket = get_client_socket_by_user_id(friend_id);
    char *username = get_user_name_by_user_id(user_id);
    if (friend_socket != -1)
    {
        Response *response;
        response->type = INVITE_FRIEND_RESPONSE;
        response->data.inviteFriendResponse.user_id = user_id;
        response->data.inviteFriendResponse.room_id = room_id;
        response->data.inviteFriendResponse.total_time = total_time;
        strcpy(response->data.inviteFriendResponse.username, username);
        ssize_t send_result = send(friend_socket, response, sizeof(Response), 0);
        if (send_result == -1)
        {
            printf("send failed\n");
        }
    }
}

void set_finding(int user_id, int is_finding)
{
    pthread_mutex_lock(&user_mutex);
    loged_in_user_t *current = get_list_online_user();

    while (current != NULL)
    {
        if (current->user_id == user_id)
        {

            current->is_finding = is_finding;
            pthread_mutex_unlock(&user_mutex);
            break;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&user_mutex);
}
int is_between(int value, int min, int max)
{
    return value >= min && value <= max;
}
int finding_match(int user_id, int elo)
{
    pthread_mutex_lock(&user_mutex);
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id != user_id && current->is_finding == 1 && is_between(current->elo, elo - ELO_THRESHOLD, elo + ELO_THRESHOLD))
        {
            pthread_mutex_unlock(&user_mutex);
            return current->user_id;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&user_mutex);
    return -1;
}
int get_client_socket_by_user_id(int user_id)
{
    pthread_mutex_lock(&user_mutex);
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            pthread_mutex_unlock(&user_mutex);
            return current->client_socket;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&user_mutex);
    return -1;
}
int get_elo_by_user_id(int user_id)
{
    pthread_mutex_lock(&user_mutex);
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            pthread_mutex_unlock(&user_mutex);
            return current->elo;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&user_mutex);
    return -1;
}

void assign_user_ids_based_on_elo(Response *response, int user_id, int opponent_id, int user_elo)
{
    if (user_elo < get_elo_by_user_id(opponent_id))
    {
        response->data.startGameData.white_user_id = user_id;
        response->data.startGameData.black_user_id = opponent_id;
        response->data.startGameData.white_elo = user_elo;
        response->data.startGameData.black_elo = get_elo_by_user_id(opponent_id);
    }
    else
    {
        response->data.startGameData.white_user_id = opponent_id;
        response->data.startGameData.black_user_id = user_id;
        response->data.startGameData.white_elo = get_elo_by_user_id(opponent_id);
        response->data.startGameData.black_elo = user_elo;
    }
}

void assign_usernames(Response *response)
{
    char *white_username = (char *)malloc(sizeof(char) * 20);
    char *black_username = (char *)malloc(sizeof(char) * 20);
    strcpy(white_username, get_user_name_by_user_id(response->data.startGameData.white_user_id));
    strcpy(black_username, get_user_name_by_user_id(response->data.startGameData.black_user_id));
    strcpy(response->data.startGameData.white_username, white_username);
    strcpy(response->data.startGameData.black_username, black_username);
}
int get_finding(int user_id)
{
    pthread_mutex_lock(&user_mutex);
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            pthread_mutex_unlock(&user_mutex);
            return current->is_finding;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&user_mutex);
    return -1;
}
void handle_finding_match(const int client_socket, const FindingMatchData *findingMatchData)
{
    time_t start_time = time(NULL);
    set_finding(findingMatchData->user_id, 1);
    char *log_msg = (char *)malloc(sizeof(char) * 100);
    while (1)
    {
        int opponent_id = finding_match(findingMatchData->user_id, findingMatchData->elo);
        if (opponent_id != -1)
        {
            if (get_finding(findingMatchData->user_id) == 0 || get_finding(opponent_id) == 0)
            {
                return;
            }
            set_finding(findingMatchData->user_id, 0);
            set_finding(opponent_id, 0);

            Response *response = (Response *)malloc(sizeof(Response));
            response->type = START_GAME;

            assign_user_ids_based_on_elo(response, findingMatchData->user_id, opponent_id, findingMatchData->elo);
            assign_usernames(response);

            int room_id = create_room_db(response->data.startGameData.white_user_id, response->data.startGameData.black_user_id, DEFAULT_TOTAL_TIME);
            response->data.startGameData.room_id = room_id;
            response->data.startGameData.total_time = DEFAULT_TOTAL_TIME;
            response->data.startGameData.status = 1;

            room_t *room = create_room(response->data.startGameData.white_user_id, DEFAULT_TOTAL_TIME);
            if (room == NULL)
            {
                sprintf(log_msg, "Failed to create room");
                Log(TAG, "e", log_msg);
                free(log_msg);
                return;
            }
            room->room_id = room_id;
            room->black_user_id = response->data.startGameData.black_user_id;
            room->white_socket = get_client_socket_by_user_id(response->data.startGameData.white_user_id);
            room->black_socket = get_client_socket_by_user_id(response->data.startGameData.black_user_id);
            room->white_user_id = response->data.startGameData.white_user_id;
            room->status = 0;
            add_room(get_list_room(), room);

            ssize_t send_result = send(client_socket, response, sizeof(Response), 0);
            if (send_result == -1)
            {
                sprintf(log_msg, "Failed to send response to client socket %d", client_socket);
                Log(TAG, "e", log_msg);
            }
            ssize_t send_result2 = send(get_client_socket_by_user_id(opponent_id), response, sizeof(Response), 0);
            if (send_result2 == -1)
            {
                sprintf(log_msg, "Failed to send response to client socket %d", get_client_socket_by_user_id(opponent_id));
                Log(TAG, "e", log_msg);
            }
            start_game_db(room_id, response->data.startGameData.white_user_id, response->data.startGameData.black_user_id, DEFAULT_TOTAL_TIME);
            sprintf(log_msg, "Found match for user %d and user %d", findingMatchData->user_id, opponent_id);
            Log(TAG, "i", log_msg);
            free(response);
            return;
        }

        time_t current_time = time(NULL);
        if (current_time - start_time >= MAX_SEARCH_TIME)
        {
            if (get_finding(findingMatchData->user_id) == 0)
            {
                return;
            }
            set_finding(findingMatchData->user_id, 0);
            Response *response = (Response *)malloc(sizeof(Response));
            response->type = START_GAME;
            response->data.startGameData.white_user_id = -1;
            response->data.startGameData.black_user_id = -1;
            response->data.startGameData.room_id = -1;
            response->data.startGameData.total_time = -1;
            sprintf(log_msg, "Failed to find match for user %d", findingMatchData->user_id);
            Log(TAG, "i", log_msg);
            ssize_t send_result = send(client_socket, response, sizeof(Response), 0);
            if (send_result == -1)
            {
                sprintf(log_msg, "Failed to send response to client socket %d", client_socket);
                Log(TAG, "e", log_msg);
            }
            free(response);
            return;
        }
        sleep(1);
    }
}

char *currtent_time()
{
    time_t current_time;
    struct tm *time_info;
    time(&current_time);
    time_info = localtime(&current_time);

    char *result = (char *)malloc(sizeof(char) * 20);
    strftime(result, 20, "%Y-%m-%d %H:%M:%S", time_info);
    return result;
}
void start_game_db(int room_id, int white_user_id, int black_user_id, int total_time)
{
    sqlite3 *db = get_database_connection();
    sqlite3_stmt *stmt;
    char *sql = UPDATE_ROOM_START_GAME;
    sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_bind_int(stmt, 1, white_user_id);
    sqlite3_bind_int(stmt, 2, black_user_id);
    sqlite3_bind_int(stmt, 3, total_time);
    char *current_time = currtent_time();
    sqlite3_bind_text(stmt, 4, current_time, strlen(current_time), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, room_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    close_database_connection(db);
    free(current_time);
}
room_t *get_room_by_id(room_t *room_list, int room_id)
{
    pthread_mutex_lock(&mutex);
    room_t *current = room_list;
    while (current != NULL)
    {
        if (current->room_id == room_id)
        {
            pthread_mutex_unlock(&mutex);
            return current;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
void handle_accept_or_decline_invitation(const int client_socket, const AcceptOrDeclineInvitationData *acceptOrDeclineInvitationData)
{
    char *log_msg = (char *)malloc(sizeof(char) * 100);
    int invited_user_socket = get_client_socket_by_user_id(acceptOrDeclineInvitationData->invited_user_id);
    Response *response = (Response *)malloc(sizeof(Response));
    if (acceptOrDeclineInvitationData->is_accept == 0)
    {
        response->type = START_GAME;
        response->data.startGameData.white_user_id = -1;
        response->data.startGameData.black_user_id = -1;
        response->data.startGameData.room_id = -1;
        response->data.startGameData.total_time = -1;
        ssize_t send_result = send(invited_user_socket, response, sizeof(Response), 0);
        if (send_result == -1)
        {
            sprintf(log_msg, "Failed to send response to client socket %d", invited_user_socket);
            Log(TAG, "e", log_msg);
        }
        sprintf(log_msg, "Declined invitation from user %d to room %d", acceptOrDeclineInvitationData->invited_user_id, acceptOrDeclineInvitationData->room_id);
        Log(TAG, "i", log_msg);
        return;
    }
    response->type = START_GAME;
    response->data.startGameData.white_user_id = acceptOrDeclineInvitationData->user_id;
    response->data.startGameData.black_user_id = acceptOrDeclineInvitationData->invited_user_id;
    response->data.startGameData.room_id = acceptOrDeclineInvitationData->room_id;
    response->data.startGameData.total_time = acceptOrDeclineInvitationData->total_time;
    char *white_username = (char *)malloc(sizeof(char) * 20);
    char *black_username = (char *)malloc(sizeof(char) * 20);
    room_t *room = get_room_by_id(get_list_room(), acceptOrDeclineInvitationData->room_id);
    if (room == NULL)
    {
        sprintf(log_msg, "Room %d not found", acceptOrDeclineInvitationData->room_id);
        Log(TAG, "e", log_msg);
        return;
    }
    room->white_socket = get_client_socket_by_user_id(response->data.startGameData.white_user_id);
    room->black_socket = get_client_socket_by_user_id(response->data.startGameData.black_user_id);
    room->white_user_id = response->data.startGameData.white_user_id;
    room->black_user_id = response->data.startGameData.black_user_id;
    strcpy(white_username, get_user_name_by_user_id(response->data.startGameData.white_user_id));
    strcpy(black_username, get_user_name_by_user_id(response->data.startGameData.black_user_id));
    strcpy(response->data.startGameData.white_username, white_username);
    strcpy(response->data.startGameData.black_username, black_username);
    response->data.startGameData.white_elo = get_elo_by_user_id(response->data.startGameData.white_user_id);
    response->data.startGameData.black_elo = get_elo_by_user_id(response->data.startGameData.black_user_id);
    response->data.startGameData.status = 0;
    ssize_t send_result = send(invited_user_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        sprintf(log_msg, "Failed to send response to client socket %d", invited_user_socket);
        Log(TAG, "e", log_msg);
    }
    ssize_t send_result2 = send(client_socket, response, sizeof(Response), 0);
    if (send_result2 == -1)
    {
        sprintf(log_msg, "Failed to send response to client socket %d", client_socket);
        Log(TAG, "e", log_msg);
    }
    sprintf(log_msg, "Accepted invitation from user %d to room %d", acceptOrDeclineInvitationData->invited_user_id, acceptOrDeclineInvitationData->room_id);
    Log(TAG, "i", log_msg);
    free(response);
    free(white_username);
    free(black_username);
}

void get_user_id_by_room_id(int room_id, int *white_user_id, int *black_user_id)
{
    pthread_mutex_lock(&mutex);
    room_t *current = get_list_room();
    while (current != NULL)
    {
        if (current->room_id == room_id)
        {
            *white_user_id = current->white_user_id;
            *black_user_id = current->black_user_id;
            pthread_mutex_unlock(&mutex);
            return;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex);
    *white_user_id = -1;
    *black_user_id = -1;
}
void update_playing_status(int user_id, int is_playing)
{
    pthread_mutex_lock(&user_mutex);
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            pthread_mutex_unlock(&user_mutex);
            current->is_playing = is_playing;
            break;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&user_mutex);
}
void handle_start_game(const int client_socket, const StartGame *startGame)
{
    int room_id = startGame->room_id;
    char *log_msg = (char *)malloc(sizeof(char) * 100);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = START_GAME;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    // print room inf
    if (room == NULL)
    {
        sprintf(log_msg, "Room %d not found", room_id);
        Log(TAG, "e", log_msg);
        free(log_msg);
        return;
    }
    ssize_t send_result = send(room->black_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        sprintf(log_msg, "Failed to send response to client socket %d", room->black_socket);
        Log(TAG, "e", log_msg);
    }
    ssize_t send_result2 = send(room->white_socket, response, sizeof(Response), 0);
    if (send_result2 == -1)
    {
        sprintf(log_msg, "Failed to send response to client socket %d", room->white_socket);
        Log(TAG, "e", log_msg);
    }
    start_game_db(room_id, room->white_user_id, room->black_user_id, room->total_time);
    update_playing_status(room->white_user_id, 1);
    update_playing_status(room->black_user_id, 1);
    sprintf(log_msg, "Started game for room %d", room_id);
    Log(TAG, "i", log_msg);
}
void move_db(int room_id, float from_x, float from_y, float to_x, float to_y, int piece_type)
{
    sqlite3 *db = get_database_connection();
    sqlite3_stmt *stmt;
    char *sql = LOG_MOVE_QUERY;
    int rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL)
    {
        printf("prepare failed\n");
        return;
    }
    sqlite3_bind_int(stmt, 1, room_id);
    sqlite3_bind_int(stmt, 2, piece_type);
    sqlite3_bind_double(stmt, 3, from_x);
    sqlite3_bind_double(stmt, 4, from_y);
    sqlite3_bind_double(stmt, 5, to_x);
    sqlite3_bind_double(stmt, 6, to_y);
    sqlite3_step(stmt);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        char *error = (char *)malloc(sizeof(char) * 100);
        sprintf(error, "Cannot prepare statement on move_db: %s\n", sqlite3_errmsg(db));
        Log(TAG, "e", error);
        free(error);
    }
    sqlite3_finalize(stmt);
    close_database_connection(db);
}

void handle_move(const int client_socket, const Move *move)
{
    if (move == NULL)
    {
        printf("move null\n");
        return;
    }
    int room_id = move->room_id;
    int opponent_socket;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    if (room == NULL)
    {
        printf("room null\n");
        return;
    }
    if (room->white_socket == client_socket)
    {
        opponent_socket = room->black_socket;
    }
    else
    {
        opponent_socket = room->white_socket;
    }
    Response *response;
    response->type = MOVE;
    response->data.move.user_id = move->user_id;
    response->data.move.room_id = move->room_id;
    response->data.move.from_x = move->from_x;
    response->data.move.from_y = move->from_y;
    response->data.move.to_x = move->to_x;
    response->data.move.to_y = move->to_y;
    response->data.move.piece_type = move->piece_type;
    response->data.move.current_time = move->current_time;
    ssize_t byte_sent = send(opponent_socket, response, sizeof(Response), 0);
    if (byte_sent == -1)
    {
        printf("send failed\n");
        return;
    }
    move_db(move->room_id, move->from_x, move->from_y, move->to_x, move->to_y, move->piece_type);
}
void update_playing(int user_id, int is_playing)
{
    pthread_mutex_lock(&user_mutex);
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            pthread_mutex_unlock(&user_mutex);
            current->is_playing = is_playing;
            break;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&user_mutex);
}
// void end_game_db(int room_id, int winner_id)
// {
//     pthread_mutex_lock(&update_end_game_mutex);
//     sqlite3 *db = get_database_connection();
//     sqlite3_stmt *stmt;
//     char *sql = UPDATE_ROOM_END_GAME;
//     sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
//     char *current_time = currtent_time();
//     sqlite3_bind_text(stmt, 1, current_time, strlen(current_time), SQLITE_STATIC);
//     sqlite3_bind_int(stmt, 2, winner_id);
//     sqlite3_bind_int(stmt, 3, room_id);

//     sqlite3_step(stmt);
//     // check for error
//     if (sqlite3_step(stmt) != SQLITE_DONE)
//     {
//         char *error = (char *)malloc(sizeof(char) * 100);
//         sprintf(error, "Cannot prepare statement on end_game_db: %s\n", sqlite3_errmsg(db));
//         Log(TAG, "e", error);
//         free(error);
//         close_database_connection(db);
//     }
//     sqlite3_finalize(stmt);
//     close_database_connection(db);
//     free(current_time);
//     pthread_mutex_lock(&update_end_game_mutex);
// }
void end_game_and_update_elo(int room_id, int winner_id, int winner_elo, int loser_id, int loser_elo)
{
    sleep(1);
    pthread_mutex_lock(&update_end_game_mutex);
    sqlite3 *db = get_database_connection();
    char *sql;
    sqlite3_stmt *stmt;
    char *current_time = currtent_time();
    char error[100];

    // Begin transaction
    sql = "BEGIN TRANSACTION;";
    sqlite3_exec(db, sql, 0, 0, 0);

    // Update end game
    sql = UPDATE_ROOM_END_GAME;
    sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, current_time, strlen(current_time), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, winner_id);
    sqlite3_bind_int(stmt, 3, room_id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sprintf(error, "Cannot prepare statement on end_game_db: %s\n", sqlite3_errmsg(db));
        Log(TAG, "e", error);
    }
    sqlite3_finalize(stmt);
    // Update winner elo
    sql = UPDATE_ELO_BY_USER_ID;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, winner_elo);
    sqlite3_bind_int(stmt, 2, winner_id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sprintf(error, "Cannot prepare statement on func elo_update: %s\n", sqlite3_errmsg(db));
        Log(TAG, "e", error);
    }
    sqlite3_finalize(stmt);

    // Update loser elo
    sql = UPDATE_ELO_BY_USER_ID;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, loser_elo);
    sqlite3_bind_int(stmt, 2, loser_id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sprintf(error, "Cannot prepare statement on func elo_update: %s\n", sqlite3_errmsg(db));
        Log(TAG, "e", error);
    }
    sqlite3_finalize(stmt);

    // Commit transaction
    sql = "COMMIT;";
    sqlite3_exec(db, sql, 0, 0, 0);
    close_database_connection(db);
    free(current_time);
    pthread_mutex_unlock(&update_end_game_mutex);
}
void handle_end_game(const int client_socket, const EndGameData *endGameData)
{
    pthread_mutex_lock(&handle_end_game_mutex);
    if (endGameData == NULL)
    {
        pthread_mutex_unlock(&handle_end_game_mutex);
        printf("end game data null\n");
        return;
    }
    int room_id = endGameData->room_id;
    int user_id = endGameData->user_id;
    int status = endGameData->status;
    int opponent_id;
    room_t *current_room = get_room_by_id(get_list_room(), room_id);
    if (current_room == NULL)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Room %d not found", room_id);
        Log(TAG, "e", log_msg);
        free(log_msg);
        pthread_mutex_unlock(&handle_end_game_mutex);
        return;
    }
    if (current_room->white_socket == client_socket)
        opponent_id = current_room->black_user_id;
    else
        opponent_id = current_room->white_user_id;
    if (current_room->status == 0)
    {
        if (status == 1)
        {
            elo_calculation(user_id, opponent_id, 1);
            end_game_and_update_elo(room_id, user_id, get_elo_by_user_id(user_id), opponent_id, get_elo_by_user_id(opponent_id));
        }
        else
        {
            elo_calculation(user_id, opponent_id, 0.5);
            end_game_and_update_elo(room_id, user_id, get_elo_by_user_id(user_id), opponent_id, get_elo_by_user_id(opponent_id));
        }
    }
    update_playing(user_id, 0);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = LOGIN_RESPONSE;
    response->data.loginResponse.is_success = 1;
    response->data.loginResponse.user_id = user_id;
    response->data.loginResponse.elo = get_elo_by_user_id(user_id);
    send(client_socket, response, sizeof(Response), 0);

    if (current_room->status == 1)
        remove_room(get_list_room(), room_id);
    else
        current_room->status++;
    pthread_mutex_unlock(&handle_end_game_mutex);
    free(response);
}

void handle_surrender(const int client_socket, const SurrenderData *surrenderData)
{
    int room_id = surrenderData->room_id;
    int user_id = surrenderData->user_id;
    int opponent_id;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    if (room == NULL)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Room %d not found", room_id);
        Log(TAG, "e", log_msg);
        free(log_msg);
        return;
    }
    if (room->white_socket == client_socket)
    {
        opponent_id = room->black_user_id;
    }
    else
    {
        opponent_id = room->white_user_id;
    }
    int opponent_socket = get_client_socket_by_user_id(opponent_id);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = SURRENDER;
    response->data.surrenderData.user_id = user_id;
    response->data.surrenderData.room_id = room_id;
    ssize_t send_result = send(opponent_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Failed to send response to client socket %d", opponent_socket);
        Log(TAG, "e", log_msg);
        free(log_msg);
    }
    free(response);
}
void handle_pause(const int client_socket, const PauseData *pauseData)
{
    int room_id = pauseData->room_id;
    int user_id = pauseData->user_id;
    int opponent_id;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    if (room == NULL)
    {
        return;
    }
    if (room->white_socket == client_socket)
    {
        opponent_id = room->black_user_id;
    }
    else
    {
        opponent_id = room->white_user_id;
    }
    int opponent_socket = get_client_socket_by_user_id(opponent_id);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = PAUSE;
    response->data.pauseData.user_id = user_id;
    response->data.pauseData.room_id = room_id;
    ssize_t send_result = send(opponent_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Failed to send response to client socket %d", opponent_socket);
        Log(TAG, "e", log_msg);
        free(log_msg);
    }
    free(response);
}
void handle_resume(const int client_socket, const ResumeData *resumeData)
{
    int room_id = resumeData->room_id;
    int user_id = resumeData->user_id;
    int opponent_id;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    if (room == NULL)
    {
        return;
    }
    if (room->white_socket == client_socket)
    {
        opponent_id = room->black_user_id;
    }
    else
    {
        opponent_id = room->white_user_id;
    }
    int opponent_socket = get_client_socket_by_user_id(opponent_id);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = RESUME;
    response->data.resumeData.user_id = user_id;
    response->data.resumeData.room_id = room_id;
    ssize_t send_result = send(opponent_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Failed to send response to client socket %d", opponent_socket);
        Log(TAG, "e", log_msg);
        free(log_msg);
    }
    free(response);
}
void handle_get_history(int client_socket, const GetGameHistory *getGameHistory)
{
    if (getGameHistory == NULL)
    {
        printf("get game history null\n");
        return;
    }
    int user_id = getGameHistory->user_id;
    sqlite3 *db = get_database_connection();
    if (db == NULL)
    {
        printf("db null\n");
        return;
    }
    sqlite3_stmt *stmt;
    char *sql = GET_HISTORY_QUERY;
    int rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL)
    {
        printf("prepare failed\n");
        return;
    }
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, user_id);
    sqlite3_bind_int(stmt, 3, user_id);
    sqlite3_bind_int(stmt, 4, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Response *response = (Response *)malloc(sizeof(Response));
        if (response == NULL)
        {
            printf("response null\n");
            return;
        }
        response->type = HISTORY_RESPONSE;
        if (sqlite3_column_type(stmt, 0) == SQLITE_NULL || sqlite3_column_type(stmt, 1) == SQLITE_NULL || sqlite3_column_type(stmt, 2) == SQLITE_NULL || sqlite3_column_type(stmt, 3) == SQLITE_NULL || sqlite3_column_type(stmt, 4) == SQLITE_NULL || sqlite3_column_type(stmt, 5) == SQLITE_NULL)
        {
            free(response);
            continue;
        }
        response->data.gameHistoryResponse.room_id = sqlite3_column_int(stmt, 0);
        strncpy(response->data.gameHistoryResponse.opponent_name, (char *)sqlite3_column_text(stmt, 1), 20);
        response->data.gameHistoryResponse.opponent_id = sqlite3_column_int(stmt, 2);
        strncpy(response->data.gameHistoryResponse.start_time, (char *)sqlite3_column_text(stmt, 3), 20);
        strncpy(response->data.gameHistoryResponse.end_time, (char *)sqlite3_column_text(stmt, 4), 20);
        response->data.gameHistoryResponse.result = sqlite3_column_int(stmt, 5);
        ssize_t send_result = send(client_socket, response, sizeof(Response), 0);
        if (send_result == -1)
        {
            printf("send result -1\n");
        }
        free(response);
    }
    sqlite3_finalize(stmt);
    close_database_connection(db);
}
void handle_accept_or_decline_draw(const int client_socket, const AcceptOrDeclineDrawData *acceptOrDeclineDrawData)
{
    int room_id = acceptOrDeclineDrawData->room_id;
    int user_id = acceptOrDeclineDrawData->user_id;
    int opponent_id;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    if (room == NULL)
    {
        return;
    }
    if (room->white_socket == client_socket)
    {
        opponent_id = room->black_user_id;
    }
    else
    {
        opponent_id = room->white_user_id;
    }
    int opponent_socket = get_client_socket_by_user_id(opponent_id);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = ACCEPT_OR_DECLINE_DRAW;
    response->data.acceptOrDeclineDrawData.user_id = user_id;
    response->data.acceptOrDeclineDrawData.room_id = room_id;
    response->data.acceptOrDeclineDrawData.is_accept = acceptOrDeclineDrawData->is_accept;
    ssize_t send_result = send(opponent_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Failed to send response to client socket %d", opponent_socket);
        Log(TAG, "e", log_msg);
        free(log_msg);
    }
    free(response);
}
void handle_draw(const int client_socket, const DrawData *drawData)
{
    int room_id = drawData->room_id;
    int user_id = drawData->user_id;
    int opponent_id;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    if (room == NULL)
    {
        return;
    }
    if (room->white_socket == client_socket)
    {
        opponent_id = room->black_user_id;
    }
    else
    {
        opponent_id = room->white_user_id;
    }
    int opponent_socket = get_client_socket_by_user_id(opponent_id);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = DRAW;
    response->data.drawData.user_id = user_id;
    response->data.drawData.room_id = room_id;
    ssize_t send_result = send(opponent_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Failed to send response to client socket %d", opponent_socket);
        Log(TAG, "e", log_msg);
        free(log_msg);
    }
    free(response);
}
void handle_replay(const int client_socket, const ReplayData *replayData)
{
    int user_id = replayData->user_id;
    int opponent_id = replayData->opponent_id;
    int opponent_socket = get_client_socket_by_user_id(opponent_id);
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = REPLAY;
    response->data.replayData.user_id = user_id;
    response->data.replayData.opponent_id = opponent_id;
    ssize_t send_result = send(opponent_socket, response, sizeof(Response), 0);
    if (send_result == -1)
    {
        char *log_msg = (char *)malloc(sizeof(char) * 100);
        sprintf(log_msg, "Failed to send response to client socket %d", opponent_socket);
        Log(TAG, "e", log_msg);
        free(log_msg);
    }
}
void handle_accept_replay(const int client_socket, const AcceptReplayData *acceptReplayData)
{
    int user_id = acceptReplayData->user_id;
    int opponent_id = acceptReplayData->opponent_id;
    int opponent_socket = get_client_socket_by_user_id(opponent_id);
    Response *response = (Response *)malloc(sizeof(Response));
    if (acceptReplayData->is_accept == 1)
    {
        int room_id = create_room_db(user_id, opponent_id, DEFAULT_TOTAL_TIME);
        response->type = START_GAME;
        response->data.startGameData.white_user_id = user_id;
        response->data.startGameData.black_user_id = opponent_id;
        response->data.startGameData.room_id = room_id;
        response->data.startGameData.total_time = DEFAULT_TOTAL_TIME;
        response->data.startGameData.status = 1;
        strcpy(response->data.startGameData.white_username, get_user_name_by_user_id(user_id));
        strcpy(response->data.startGameData.black_username, get_user_name_by_user_id(opponent_id));
        response->data.startGameData.white_elo = get_elo_by_user_id(user_id);
        response->data.startGameData.black_elo = get_elo_by_user_id(opponent_id);
        ssize_t send_result1 = send(opponent_socket, response, sizeof(Response), 0);
        if (send_result1 == -1)
        {
            char *log_msg = (char *)malloc(sizeof(char) * 100);
            sprintf(log_msg, "Failed to send response to client socket %d", client_socket);
            Log(TAG, "e", log_msg);
            free(log_msg);
        }
        ssize_t send_result = send(client_socket, response, sizeof(Response), 0);
        if (send_result == -1)
        {
            char *log_msg = (char *)malloc(sizeof(char) * 100);
            sprintf(log_msg, "Failed to send response to client socket %d", client_socket);
            Log(TAG, "e", log_msg);
            free(log_msg);
        }
        start_game_db(room_id, user_id, opponent_id, DEFAULT_TOTAL_TIME);
    }
    else
    {
        response->type = START_GAME;
        response->data.startGameData.white_user_id = -1;
        response->data.startGameData.black_user_id = -1;
        response->data.startGameData.room_id = -1;
        response->data.startGameData.total_time = -1;
        ssize_t send_result = send(opponent_socket, response, sizeof(Response), 0);
        if (send_result == -1)
        {
            char *log_msg = (char *)malloc(sizeof(char) * 100);
            sprintf(log_msg, "Failed to send response to client socket %d", client_socket);
            Log(TAG, "e", log_msg);
            free(log_msg);
        }
    }
    free(response);
}
void handle_get_move_history(int client_socket, const GetMoveHistory *getMoveHistory)
{
    int room_id = getMoveHistory->room_id;
    sqlite3 *db = get_database_connection();
    sqlite3_stmt *stmt;
    char *sql = GET_MOVE_HISTORY_QUERY;
    int rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL)
    {
        printf("prepare failed\n");
        return;
    }
    sqlite3_bind_int(stmt, 1, room_id);
    // array of move
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = MOVE_HISTORY_RESPONSE;
    MoveHistory *moveHistory = (MoveHistory *)malloc(sizeof(MoveHistory));
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        moveHistory->room_id = room_id;
        moveHistory->move_id = sqlite3_column_int(stmt, 0);
        moveHistory->piece_type = sqlite3_column_int(stmt, 1);
        moveHistory->from_x = sqlite3_column_double(stmt, 2);
        moveHistory->from_y = sqlite3_column_double(stmt, 3);
        moveHistory->to_x = sqlite3_column_double(stmt, 4);
        moveHistory->to_y = sqlite3_column_double(stmt, 5);
        response->data.moveHistory = *moveHistory;
        ssize_t bytes_sent = send(client_socket, response, sizeof(Response), 0);
        if (bytes_sent <= 0)
        {
            printf("bytes sent <= 0\n");
            return;
        }
        sleep(1);
    }
    moveHistory->room_id = -1;
    moveHistory->move_id = -1;
    moveHistory->piece_type = -1;
    moveHistory->from_x = -1;
    moveHistory->from_y = -1;
    moveHistory->to_x = -1;
    moveHistory->to_y = -1;
    response->data.moveHistory = *moveHistory;
    ssize_t bytes_sent = send(client_socket, response, sizeof(Response), 0);
    if (bytes_sent <= 0)
    {
        printf("bytes sent <= 0\n");
        return;
    }
    free(response);
    free(moveHistory);
}