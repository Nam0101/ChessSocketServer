#ifndef GAME_H
#define GAME_H
#include "../server.h"

typedef struct
{
    int black_user_id;
    int white_user_id;
    int total_time; // on seconds
} room_t;

room_t *create_room(int white_user_id, int total_time);
void handle_create_room(const int client_socket, const CreateRoomData *createRoomData);
void handle_finding_match(const int client_socket, const FindingMatchData *findingMatchData);
void handle_invite_friend(const int client_socket, const InviteFriendData *inviteFriendData);
int get_client_socket_by_user_id(int user_id);
void handle_accept_or_decline_invitation(const int client_socket, const AcceptOrDeclineInvitationData *acceptOrDeclineInvitationData);
int get_elo_by_user_id(int user_id);
#endif