#ifndef HISTORY_H
#define HISTORY_H
typedef struct
{
    int user_id;
} GetGameHistory;
typedef struct
{
    int user_id;
    int number_of_game;
} NumberOfGameHistory;
typedef struct
{
    int user_id;
    int room_id;
    int result;    // 0: lose, 1: win, 2: draw
    char start_time[20];
    char end_time[20];
    char opponent_name[20];
    int opponent_id;
} GameHistoryResponse;
typedef struct
{
    int user_id; // your id
    int room_id; // room id which you want to get history
} GetMoveHistory;
typedef struct
{
    int room_id;
    int move_id;
    int piece_type;
    float from_x;
    float from_y;
    float to_x;
    float to_y;
} MoveHistory;
#endif