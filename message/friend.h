#ifndef FRIEND_H
#define FRIEND_H

typedef struct
{
    int user_id;
    int friend_id;
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
    short friend_id[10];
} OnlineFriendsResponse;
typedef struct
{
    int user_id;
    int friend_id;
    int room_id;
    int total_time;
} InviteFriendResponse;

typedef struct
{
    short is_accept;
} AcceptOrDeclineInvitationData;

typedef struct
{
    short is_success;
    char message_code;
} AddFriendResponse;
#endif