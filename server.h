#ifndef SERVER_H
#define SERVER_H
#include "message/login.h"
#include "message/register.h"
#include "message/logout.h"
#include "message/friend.h"
#include "message/room.h"
#define PORT 12345
#define BACKLOG 100
#define MAX_BUFFER_SIZE 4096
#define THREAD_POOL_SIZE 8

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
    ACCEPT_OR_DECLINE_INVITATION,
    START_GAME
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
        StartGameData startGameData;
    } data;
} Response;

#endif
