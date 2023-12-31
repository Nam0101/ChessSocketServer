#ifndef FRIEND_H
#define FRIEND_H

typedef struct
{
    int user_id;
    char username[20];
} AddFriendData;

typedef struct
{
    int user_id;
} GetOnlineFriendsData;
typedef struct
{
    int user_id;
    int friend_id;
    int room_id;
    int total_time;
} InviteFriendData;
typedef struct
{
    short number_of_friends;
} OnlineFriendsResponse;
typedef struct
{
    int friend_id;
    int elo;
    int is_online;
    int is_playing;
    char username[20];
} FriendDataResponse;

typedef struct
{
    int user_id;
    int friend_id;
    int room_id;
    int total_time;
    char username[20];
} InviteFriendResponse;

typedef struct
{
    char is_accept;
    int user_id;
    int room_id;
    int invited_user_id;
    int total_time;
} AcceptOrDeclineInvitationData;

typedef struct
{
    short is_success;
    char message_code;
    int friend_id;
    int elo;
    int is_online;
    int is_playing;
} AddFriendResponse;
typedef struct{
    int user_id;
}GetTopPlayerData;

typedef struct{
    int user_id;
    int rank;
    int elo;
    char username[20];
}TopPlayerDataResponse;
typedef struct{
    int user_id;
    int friend_id;
    char message[70];
}ChatData;
#endif