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
#endif