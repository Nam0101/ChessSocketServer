#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "../server.h"
#include "../user/user.h"
#include "../database/database.h"
#define MAX_SEARCH_TIME 10
#define ELO_THRESHOLD 100 // elo difference between 2 players
#define CREATE_ROOM "INSERT INTO room (white_user_id, black_user_id, total_time) VALUES (%d, %d, %d);"
room_t *create_room(int white_user_id, int total_time)
{
    room_t *room = (room_t *)malloc(sizeof(room_t));
    room->white_user_id = white_user_id;
    room->black_user_id = -1;
    room->total_time = total_time;
    return room;
}
void send_reponse(const int client_socket, const Response *response)
{
    int bytes_sent = send(client_socket, response, sizeof(Response), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
}

int create_room_db(int white_user_id, int black_user_id, int total_time)
{
    sqlite3 *db = get_database_connection();
    char *sql = (char *)malloc(sizeof(char) * 100);
    sprintf(sql, CREATE_ROOM, white_user_id, black_user_id, total_time);
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        close_database_connection(db);
        return -1; // Trả về giá trị âm để chỉ ra lỗi
    }

    // Sau khi thêm dữ liệu, thực hiện truy vấn SELECT để lấy room.id
    int last_inserted_id = sqlite3_last_insert_rowid(db);

    close_database_connection(db);

    return last_inserted_id; // Trả về room.id
}

void handle_create_room(const int client_socket, const CreateRoomData *createRoomData)
{
    room_t *room = create_room(createRoomData->user_id, createRoomData->total_time);
    if (room != NULL)
    {
        Response *response = (Response *)malloc(sizeof(Response));
        response->type = CREATE_ROOM_RESPONSE;
        response->data.createRoomResponse.is_success = 1;
        response->data.createRoomResponse.message_code = 0;
        int room_id = create_room_db(room->white_user_id, room->black_user_id, room->total_time);
        response->data.createRoomResponse.room_id = room_id;
        send_reponse(client_socket, response);
    }
    else
    {
        Response *response = (Response *)malloc(sizeof(Response));
        response->type = CREATE_ROOM_RESPONSE;
        response->data.createRoomResponse.is_success = 0;
        response->data.createRoomResponse.message_code = 1;
        send_reponse(client_socket, response);
    }
}

void send_invite_friend(int friend_socket, int user_id, int room_id, int total_time)
{
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = INVITE_FRIEND_RESPONSE;
    response->data.inviteFriendResponse.user_id = user_id;
    response->data.inviteFriendResponse.room_id = room_id;
    response->data.inviteFriendResponse.total_time = total_time;
    send_reponse(friend_socket, response);
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
        send_invite_friend(friend_socket, user_id, room_id, inviteFriendData->total_time);
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
void handle_finding_match(const int client_socket, const FindingMatchData *findingMatchData)
{
    time_t start_time = time(NULL);
    printf("Finding match for user %d with elo:%d\n", findingMatchData->user_id, findingMatchData->elo);

    set_finding(findingMatchData->user_id, 1);

    while (1)
    {
        int opponent_id = finding_match(findingMatchData->user_id, findingMatchData->elo);
        if (opponent_id != -1)
        {
            set_finding(findingMatchData->user_id, 0);
            set_finding(opponent_id, 0);
            handle_finding_match_response(client_socket, findingMatchData->user_id, opponent_id);
            handle_finding_match_response(get_client_socket_by_user_id(opponent_id), opponent_id, findingMatchData->user_id);
            break;
        }

        time_t current_time = time(NULL);
        if (current_time - start_time >= MAX_SEARCH_TIME)
        {
            set_finding(findingMatchData->user_id, 0);
            handle_finding_match_response(client_socket, findingMatchData->user_id, -1);
            break;
        }
        sleep(1);
    }
}
void handle_finding_match_response(const int client_socket, const int user_id, const int opponent_id)
{
    Response *response = (Response *)malloc(sizeof(Response));
    response->type = FINDING_MATCH_RESPONSE;
    response->data.findingMatchResponse.is_success = (opponent_id != -1) ? 1 : 0;
    response->data.findingMatchResponse.opponent_id = opponent_id;

    send_reponse(client_socket, response);
    free(response);
}