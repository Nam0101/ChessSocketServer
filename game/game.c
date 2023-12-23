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
#define DEFAULT_TOTAL_TIME 600
#define MAX_SEARCH_TIME 10
#define ELO_THRESHOLD 100 // elo difference between 2 players
#define CREATE_ROOM "INSERT INTO room (white_user_id, black_user_id, total_time) VALUES (?, ?, ?);"
#define UPDATE_ROOM_START_GAME "UPDATE room SET white_user_id = ?, black_user_id = ?, total_time = ?, start_time = ? WHERE id = ?;"
#define LOG_MOVE_QUERY "INSERT INTO move (room_id, piece_id, from_x, from_y, to_x, to_y) VALUES(?, ?, ?, ?, ?, ?);"
// mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// list of room
static room_t *list_room = NULL;
room_t *create_room(int white_user_id, int total_time)
{
    room_t *room = (room_t *)malloc(sizeof(room_t));
    room->white_user_id = white_user_id;
    room->black_user_id = -1;
    room->total_time = total_time;
    room->next = NULL;
    return room;
}
void send_reponse(const int client_socket, const Response *response)
{
    send(client_socket, response, sizeof(Response), 0);
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
        add_room(get_list_room(), room);
        response->data.createRoomResponse.room_id = room_id;
        send_reponse(client_socket, response);
    }
    else
    {
        response->data.createRoomResponse.is_success = 0;
        response->data.createRoomResponse.message_code = 1;
        send_reponse(client_socket, response);
    }
    free(response);
}

void send_invite_friend(int friend_socket, int user_id, int room_id, int total_time)
{
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = INVITE_FRIEND_RESPONSE;
    response->data.inviteFriendResponse.user_id = user_id;
    response->data.inviteFriendResponse.room_id = room_id;
    response->data.inviteFriendResponse.total_time = total_time;
    send_reponse(friend_socket, response);
    free(response);
}
void handle_invite_friend(const int client_socket, const InviteFriendData *inviteFriendData)
{
    int friend_id = inviteFriendData->friend_id;
    int room_id = inviteFriendData->room_id;
    int user_id = inviteFriendData->user_id;
    int total_time = inviteFriendData->total_time;
    int friend_socket = get_client_socket_by_user_id(friend_id);
    if (friend_socket != -1)
    {
        send_invite_friend(friend_socket, user_id, room_id, total_time);
    }
}

void set_finding(int user_id, int is_finding)
{
    loged_in_user_t *current = get_list_online_user();

    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            current->is_finding = is_finding;
            break;
        }
        current = current->next;
    }
}
int is_between(int value, int min, int max)
{
    return value >= min && value <= max;
}
int finding_match(int user_id, int elo)
{
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id != user_id && current->is_finding == 1 && is_between(current->elo, elo - ELO_THRESHOLD, elo + ELO_THRESHOLD))
        {
            return current->user_id;
        }
        current = current->next;
    }
    return -1;
}
int get_client_socket_by_user_id(int user_id)
{
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            return current->client_socket;
        }
        current = current->next;
    }
    return -1;
}
int get_elo_by_user_id(int user_id)
{
    loged_in_user_t *current = get_list_online_user();
    while (current != NULL)
    {
        if (current->user_id == user_id)
        {
            return current->elo;
        }
        current = current->next;
    }
    return -1;
}

void assign_user_ids_based_on_elo(Response *response, int user_id, int opponent_id, int user_elo)
{
    if (user_elo < get_elo_by_user_id(opponent_id))
    {
        response->data.startGameData.white_user_id = user_id;
        response->data.startGameData.black_user_id = opponent_id;
    }
    else
    {
        response->data.startGameData.white_user_id = opponent_id;
        response->data.startGameData.black_user_id = user_id;
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

void handle_finding_match(const int client_socket, const FindingMatchData *findingMatchData)
{
    time_t start_time = time(NULL);
    set_finding(findingMatchData->user_id, 1);

    while (1)
    {
        int opponent_id = finding_match(findingMatchData->user_id, findingMatchData->elo);
        if (opponent_id != -1)
        {
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
            room->room_id = room_id;
            room->black_user_id = response->data.startGameData.black_user_id;
            room->white_socket = get_client_socket_by_user_id(response->data.startGameData.white_user_id);
            room->black_socket = get_client_socket_by_user_id(response->data.startGameData.black_user_id);
            room->white_user_id = response->data.startGameData.white_user_id;

            add_room(get_list_room(), room);

            send_reponse(client_socket, response);
            send_reponse(get_client_socket_by_user_id(opponent_id), response);
            start_game_db(room_id, response->data.startGameData.white_user_id, response->data.startGameData.black_user_id, DEFAULT_TOTAL_TIME);
            free(response);
            return;
        }

        time_t current_time = time(NULL);
        if (current_time - start_time >= MAX_SEARCH_TIME)
        {
            set_finding(findingMatchData->user_id, 0);
            Response *response = (Response *)malloc(sizeof(Response));
            response->type = START_GAME;
            response->data.startGameData.white_user_id = -1;
            response->data.startGameData.black_user_id = -1;
            response->data.startGameData.room_id = -1;
            response->data.startGameData.total_time = -1;
            send_reponse(client_socket, response);
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
    room_t *current = room_list;
    while (current != NULL)
    {
        if (current->room_id == room_id)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
void handle_accept_or_decline_invitation(const int client_socket, const AcceptOrDeclineInvitationData *acceptOrDeclineInvitationData)
{
    int invited_user_socket = get_client_socket_by_user_id(acceptOrDeclineInvitationData->invited_user_id);
    Response *response = (Response *)malloc(sizeof(Response));
    if (acceptOrDeclineInvitationData->is_accept == 0)
    {
        response->type = ACCEPT_OR_DECLINE_INVITATION;
        response->data.acceptOrDeclineInvitationData.is_accept = 0;
        response->data.acceptOrDeclineInvitationData.user_id = acceptOrDeclineInvitationData->user_id;
        send_reponse(invited_user_socket, response);
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
    room->white_socket = get_client_socket_by_user_id(response->data.startGameData.white_user_id);
    room->black_socket = get_client_socket_by_user_id(response->data.startGameData.black_user_id);
    room->white_user_id = response->data.startGameData.white_user_id;
    room->black_user_id = response->data.startGameData.black_user_id;
    strcpy(white_username, get_user_name_by_user_id(response->data.startGameData.white_user_id));
    strcpy(black_username, get_user_name_by_user_id(response->data.startGameData.black_user_id));
    strcpy(response->data.startGameData.white_username, white_username);
    strcpy(response->data.startGameData.black_username, black_username);
    response->data.startGameData.status = 0;
    send_reponse(invited_user_socket, response);
    send_reponse(client_socket, response);
    free(response);
    free(white_username);
    free(black_username);
}

void get_user_id_by_room_id(int room_id, int *white_user_id, int *black_user_id)
{
    room_t *current = get_list_room();
    while (current != NULL)
    {
        if (current->room_id == room_id)
        {
            *white_user_id = current->white_user_id;
            *black_user_id = current->black_user_id;
            return;
        }
        current = current->next;
    }

    *white_user_id = -1;
    *black_user_id = -1;
}

void handle_start_game(const int client_socket, const StartGame *startGame)
{
    printf("start game\n");
    int room_id = startGame->room_id;
    int room_owner_id = startGame->user_id;
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = START_GAME;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    // print room inf
    if (room == NULL)
    {
        printf("room is null\n");
        return;
    }
    printf("room id: %d\n", room->room_id);
    printf("white user id: %d\n", room->white_user_id);
    printf("black user id: %d\n", room->black_user_id);
    printf("white socket: %d\n", room->white_socket);
    printf("black socket: %d\n", room->black_socket);
    printf("total time: %d\n", room->total_time);
    send_reponse(room->black_socket, response);
    send_reponse(room->white_socket, response);
    start_game_db(room_id, room->white_user_id, room->black_user_id, room->total_time);
}
void move_db(int room_id, float from_x, float from_y, float to_x, float to_y, int piece_type)
{
    sqlite3 *db = get_database_connection();
    sqlite3_stmt *stmt;
    char *sql = LOG_MOVE_QUERY;
    sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    sqlite3_bind_int(stmt, 1, room_id);
    sqlite3_bind_int(stmt, 2, piece_type);
    sqlite3_bind_double(stmt, 3, from_x);
    sqlite3_bind_double(stmt, 4, from_y);
    sqlite3_bind_double(stmt, 5, to_x);
    sqlite3_bind_double(stmt, 6, to_y);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    close_database_connection(db);
}

void handle_move(const int client_socket, const Move *move)
{
    printf("handle move\n");
    printf("room id: %d\n", move->room_id);
    printf("user id: %d\n", move->user_id);
    printf("from x: %f\n", move->from_x);
    printf("from y: %f\n", move->from_y);
    printf("to x: %f\n", move->to_x);
    printf("to y: %f\n", move->to_y);
    printf("piece type: %d\n", move->piece_type);
    printf("current time: %d\n", move->current_time);
    int room_id = move->room_id;
    int opponent_socket;
    room_t *room = get_room_by_id(get_list_room(), room_id);
    if (room->white_socket == client_socket)
    {
        opponent_socket = room->black_socket;
    }
    else
    {
        opponent_socket = room->white_socket;
    }
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = MOVE;
    response->data.move.user_id = move->user_id;
    response->data.move.room_id = move->room_id;
    response->data.move.from_x = move->from_x;
    response->data.move.from_y = move->from_y;
    response->data.move.to_x = move->to_x;
    response->data.move.to_y = move->to_y;
    response->data.move.piece_type = move->piece_type;
    response->data.move.current_time = move->current_time;
    send_reponse(opponent_socket, response);
    move_db(move->room_id, move->from_x, move->from_y, move->to_x, move->to_y, move->piece_type);
    free(response);
}