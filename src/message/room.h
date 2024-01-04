#ifndef ROOM_H
#define ROOM_H

typedef struct
{
    int user_id;
    int total_time;
} CreateRoomData;
typedef struct
{
    int elo;
    int user_id;
} FindingMatchData;

typedef struct
{
    short is_success;
    char message_code;
    int room_id;
} CreateRoomResponse;
typedef struct
{
    int white_user_id;
    int black_user_id;
    int white_elo;
    int black_elo;
    char white_username[20];
    char black_username[20];
    int room_id;
    int total_time;
    char status;
} StartGameData;
typedef struct
{
    int room_id;
    int user_id;
    short status; // 0: lose, 1: win, 2: draw
} EndGameData;
typedef struct
{
    int user_id;
    int room_id;
} StartGame;
typedef struct
{
    int user_id;
    int room_id;
    float from_x;
    float from_y;
    float to_x;
    float to_y;
    int piece_type;
    int current_time;
} Move;
typedef struct
{
    int user_id;
    int room_id;
} JoinRoomData;
typedef struct
{
    int user_id;
    int room_id;
} SurrenderData;
typedef struct
{
    int user_id;
    int room_id;
} PauseData;
typedef struct
{
    int user_id;
    int room_id;
} ResumeData;

#endif