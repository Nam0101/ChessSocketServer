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
    int opponent_id;
} FindingMatchResponse;
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
    int room_id;
    int total_time;
} StartGameData;
#endif