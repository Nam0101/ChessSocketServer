#ifndef SERVER_H
#define SERVER_H
#include "message/login.h"
#include "message/register.h"
#include "message/logout.h"
#include "message/friend.h"
#include "message/room.h"
#include "message/history.h"
#define PORT 8888
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
    FRIEND_DATA_RESPONSE,
    CREATE_ROOM,
    CREATE_ROOM_RESPONSE,
    INVITE_FRIEND,
    INVITE_FRIEND_RESPONSE,
    FINDING_MATCH,
    FINDING_MATCH_RESPONSE,
    ACCEPT_OR_DECLINE_INVITATION,
    START_GAME_DATA,
    MOVE,
    START_GAME,
    END_GAME,
    SURRENDER,
    PAUSE,
    RESUME,
    DRAW,
    ACCEPT_OR_DECLINE_DRAW,
    GET_HISTORY,
    NUMBER_OF_HISTORY,
    HISTORY_RESPONSE,
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
        StartGame startGame;
        Move move;
        EndGameData endGameData;
        SurrenderData surrenderData;
        PauseData pauseData;
        ResumeData resumeData;
        GetGameHistory getGameHistory;
        DrawData drawData;
        AcceptOrDeclineDrawData acceptOrDeclineDrawData;
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
        InviteFriendResponse inviteFriendResponse;
        AcceptOrDeclineInvitationData acceptOrDeclineInvitationData;
        StartGameData startGameData;
        FriendDataResponse friendDataResponse;
        StartGame startGame;
        Move move;
        SurrenderData surrenderData;
        PauseData pauseData;
        ResumeData resumeData;
        NumberOfGameHistory numberOfGameHistory;
        GameHistoryResponse gameHistoryResponse;
        DrawData drawData;
        AcceptOrDeclineDrawData acceptOrDeclineDrawData;
    } data;
} Response;

#endif
