#ifndef GAME_H
#define GAME_H
#include "../server.h"

typedef struct room_t
{
    int black_user_id;
    int white_user_id;
    int total_time;
    int room_id;
    int white_socket;
    int black_socket;
    int status; // 1: playing, 2: 1 user sent end game, 3: 2 user sent end game
    struct room_t *next;
} room_t;
// list of room

room_t *create_room(int white_user_id, int total_time);
void handle_create_room(const int client_socket, const CreateRoomData *createRoomData);
void handle_finding_match(const int client_socket, const FindingMatchData *findingMatchData);
void handle_invite_friend(const int client_socket, const InviteFriendData *inviteFriendData);
int get_client_socket_by_user_id(int user_id);
void handle_accept_or_decline_invitation(const int client_socket, const AcceptOrDeclineInvitationData *acceptOrDeclineInvitationData);
int get_elo_by_user_id(int user_id);
void handle_start_game(const int client_socket, const StartGame *StartGame);
void get_user_id_by_room_id(int room_id, int *white_user_id, int *black_user_id);
void handle_move(const int client_socket, const Move *move);
void start_game_db(int room_id, int white_user_id, int black_user_id, int total_time);
void handle_end_game(const int client_socket, const EndGameData *endGameData);
void handle_surrender(const int client_socket, const SurrenderData *surrenderData);
void handle_pause(const int client_socket, const PauseData *pauseData);
void handle_resume(const int client_socket, const ResumeData *resumeData);
void handle_draw(const int client_socket, const DrawData *drawData);
void handle_accept_or_decline_draw(const int client_socket, const AcceptOrDeclineDrawData *acceptOrDeclineDrawData);
void handle_replay(const int client_socket, const ReplayData *replayData);
void handle_accept_replay(const int client_socket, const AcceptReplayData *acceptReplayData);
void handle_get_history(int client_socket, const GetGameHistory *getGameHistory);
void handle_get_move_history(int client_socket, const GetMoveHistory *getMoveHistory);
#endif