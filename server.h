#ifndef SERVER_H
#define SERVER_H

#define PORT 12345
#define BACKLOG 100
#define MAX_BUFFER_SIZE 4096
#define THREAD_POOL_SIZE 8

typedef struct
{
    char username[20];
    char password[10];
} LoginData;

typedef struct
{
    char username[20];
    char password[10];
} RegisterData;
typedef struct
{
    int user_id;
} ExitData;
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
    short is_success;
    int user_id;
    int elo;
    char message_code;
} LoginResponse;
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
} RegisterResponse;
typedef struct
{
    short is_success;
    char message_code;
} AddFriendResponse;
typedef struct
{
    short is_success;
    char message_code;
    int room_id;
} CreateRoomResponse;
typedef enum
{
    LOGIN,
    REGISTER,
    LOGIN_RESPONSE,
    REGISTER_RESPONSE,
    EXIT,
    ADD_FRIEND,
    ADD_FRIEND_RESPONSE,
    GET_ONLINE_FRIENDS,
    ONLINE_FRIENDS_RESPONSE,
    CREATE_ROOM,
    CREATE_ROOM_RESPONSE,
    INVITE_FRIEND,
    INVITE_FRIEND_RESPONSE,
    FINDING_MATCH,
    FINDING_MATCH_RESPONSE,
    ACCEPT_OR_DECLINE_INVITATION
} MessageType;

typedef struct
{
    MessageType type;
    union
    {
        LoginData loginData;
        RegisterData registerData;
        ExitData exitData;
        AddFriendData addFriendData;
        GetOnlineFriendsData getOnlineFriendsData;
        CreateRoomData createRoomData;
        FindingMatchData findingMatchData;
        InviteFriendData inviteFriendData;
        AcceptOrDeclineInvitationData acceptOrDeclineInvitationData;
    } data;
} Message;
typedef struct
{
    MessageType type;
    union
    {
        LoginResponse loginResponse;
        RegisterResponse registerResponse;
        AddFriendResponse addFriendResponse;
        OnlineFriendsResponse onlineFriendsResponse;
        CreateRoomResponse createRoomResponse;
        FindingMatchResponse findingMatchResponse;
        InviteFriendResponse inviteFriendResponse;
        AcceptOrDeclineInvitationData acceptOrDeclineInvitationData;
    } data;
} Response;

#endif
