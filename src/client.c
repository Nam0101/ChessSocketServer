#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/sha.h>
#include "server.h"
#define PORT 8888
#define MAX_BUFFER_SIZE 4096
#define SERVER_ERROR 'E'
#define USERNAME_EXISTS 'U'
#define REGISTER_SUCCESS 'S'
#define USER_LOGED_IN 'L'
#define USERNAME_PASSWORD_WRONG 'W'
#define ALREADY_FRIEND 'A'
#define FRIEND_ID_NOT_FOUND 'F'
void check(int code)
{
    if (code == -1)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }
}
int user_id, elo, room_id, mtotal_time;
void login(int client_socket)
{
    Message message;
    message.type = LOGIN; // or REGISTER
    char username[50];
    char password[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    strcpy(message.data.loginData.username, username);
    strcpy(message.data.loginData.password, password);
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    // get current time
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }

    // // Receive response from server
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    // get current time

    // in ra thời gian server xử lý đến ms
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case LOGIN_RESPONSE:
        if (response.data.loginResponse.is_success == 1)
        {
            printf("Login success\n");
            printf("User id: %d\n", response.data.loginResponse.user_id);
            printf("Elo: %d\n", response.data.loginResponse.elo);
            user_id = response.data.loginResponse.user_id;
            elo = response.data.loginResponse.elo;
        }
        else
        {
            if (response.data.loginResponse.message_code == USER_LOGED_IN)
            {
                printf("User loged in\n");
            }
            else if (response.data.loginResponse.message_code == USERNAME_PASSWORD_WRONG)
            {
                printf("Username or password wrong\n");
            }
            else
            {
                printf("Server error\n");
            }
        }
        break;

    default:
        printf("Invalid response\n");
        break;
    }
}

void sendRegister(int client_socket)
{
    Message message;
    message.type = REGISTER; // or REGISTER
    char username[50];
    char password[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    strcpy(message.data.registerData.username, username);
    strcpy(message.data.registerData.password, password);
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }

    // // Receive response from server
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case REGISTER_RESPONSE:
        if (response.data.registerResponse.is_success == 1)
        {
            printf("Register success\n");
        }
        else
        {
            if (response.data.registerResponse.message_code == USERNAME_EXISTS)
            {
                printf("Username already exists\n");
            }
            else
            {
                printf("Server error\n");
            }
        }
        break;

    default:
        printf("Invalid response\n");
        break;
    }
}

void logout(int client_socket)
{
    Message message;
    message.type = EXIT;
    message.data.exitData.user_id = user_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
}

void addFriend(int client_socket)
{
    Message message;
    message.type = ADD_FRIEND;
    message.data.addFriendData.user_id = user_id;
    char *friend_username = (char *)malloc(20);
    printf("Enter friend username: ");
    scanf("%s", friend_username);
    strcpy(message.data.addFriendData.username, friend_username);
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case ADD_FRIEND_RESPONSE:
        if (response.data.addFriendResponse.is_success == 1)
        {
            printf("Add friend success\n");
            printf("Friend id: %d\n", response.data.addFriendResponse.friend_id);
            printf("Elo: %d\n", response.data.addFriendResponse.elo);
            printf("Is online: %d\n", response.data.addFriendResponse.is_online);
            printf("Is playing: %d\n", response.data.addFriendResponse.is_playing);
        }
        else
        {
            if (response.data.addFriendResponse.message_code == FRIEND_ID_NOT_FOUND)
            {
                printf("Friend id not found\n");
            }
            else if (response.data.addFriendResponse.message_code == ALREADY_FRIEND)
            {
                printf("Already friend\n");
            }
            else
            {
                printf("Server error\n");
            }
        }
        break;
    }
}

void get_list_online_user(int client_socket)
{
    Message message;
    message.type = GET_ONLINE_FRIENDS;
    message.data.getOnlineFriendsData.user_id = user_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case ONLINE_FRIENDS_RESPONSE:
        printf("Number of friends: %d\n", response.data.onlineFriendsResponse.number_of_friends);
        for (int i = 0; i < response.data.onlineFriendsResponse.number_of_friends; i++)
        {
            Response *friend_i = (Response *)malloc(sizeof(Response));
            bytes_received = recv(client_socket, friend_i, sizeof(Response), 0);
            if (bytes_received <= 0)
            {
                printf("Connection closed\n");
            }
            else
            {
                printf("Received: %d bytes\n", bytes_received);
            }
            printf("Friend id: %d\n", friend_i->data.friendDataResponse.friend_id);
            printf("Elo: %d\n", friend_i->data.friendDataResponse.elo);
            printf("Is online: %d\n", friend_i->data.friendDataResponse.is_online);
            printf("Is playing: %d\n", friend_i->data.friendDataResponse.is_playing);
            printf("Username: %s\n", friend_i->data.friendDataResponse.username);
        }
        break;
    }
}
void create_room(int client_socket)
{
    Message message;
    message.type = CREATE_ROOM;
    message.data.createRoomData.user_id = user_id;
    int total_time;
    printf("Enter total time: ");
    scanf("%d", &total_time);
    message.data.createRoomData.total_time = total_time;
    mtotal_time = message.data.createRoomData.total_time;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case CREATE_ROOM_RESPONSE:
        if (response.data.createRoomResponse.is_success == 1)
        {
            printf("Create room success\n");
            printf("Room id: %d\n", response.data.createRoomResponse.room_id);
            room_id = response.data.createRoomResponse.room_id;
        }
        else
        {
            printf("Create room failed\n");
        }
        break;
    }
}

void finding_match(int client_socket)
{
    Message message;
    message.type = FINDING_MATCH;
    message.data.findingMatchData.user_id = user_id;
    message.data.findingMatchData.elo = elo;
    printf("Finding match with elo:%d\n", elo);
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case START_GAME:
        printf("Start game\n");
        if (response.data.startGameData.white_user_id == -1)
        {
            printf("Response is timeout\n");
            break;
        }
        printf("White user id: %d\n", response.data.startGameData.white_user_id);
        printf("Black user id: %d\n", response.data.startGameData.black_user_id);
        printf("Black username: %s\n", response.data.startGameData.black_username);
        printf("White username: %s\n", response.data.startGameData.white_username);
        printf("Room id: %d\n", response.data.startGameData.room_id);
        room_id = response.data.startGameData.room_id;
        printf("Total time: %d\n", response.data.startGameData.total_time);
        printf("Status: %c\n", response.data.startGameData.status);

        break;
    }
}
void invite_friend(int client_socket)
{
    Message message;
    message.type = INVITE_FRIEND;
    message.data.inviteFriendData.user_id = user_id;
    int friend_id;
    printf("Enter friend id: ");
    scanf("%d", &friend_id);
    message.data.inviteFriendData.friend_id = friend_id;
    message.data.inviteFriendData.room_id = room_id;
    message.data.inviteFriendData.total_time = mtotal_time;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case START_GAME:
        printf("Start game\n");
        printf("White user id: %d\n", response.data.startGameData.white_user_id);
        printf("Black user id: %d\n", response.data.startGameData.black_user_id);
        printf("Room id: %d\n", response.data.startGameData.room_id);
        printf("Total time: %d\n", response.data.startGameData.total_time);
        printf("Black username: %s\n", response.data.startGameData.black_username);
        printf("White username: %s\n", response.data.startGameData.white_username);
        printf("Status: %c\n", response.data.startGameData.status);
        break;

        break;
    }
}
void waiting_for_invite(int client_socket)
{
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    if (response.type == INVITE_FRIEND_RESPONSE)
    {
        printf("Invite from user %d\n", response.data.inviteFriendResponse.user_id);
        printf("Room id: %d\n", response.data.inviteFriendResponse.room_id);
        printf("Total time: %d\n", response.data.inviteFriendResponse.total_time);
        printf("Username: %s\n", response.data.inviteFriendResponse.username);
        int is_accept;
        printf("Enter 1 to accept, 0 to decline: ");
        scanf("%d", &is_accept);
        Message message;
        message.type = ACCEPT_OR_DECLINE_INVITATION;
        message.data.acceptOrDeclineInvitationData.is_accept = is_accept;
        message.data.acceptOrDeclineInvitationData.user_id = user_id;
        message.data.acceptOrDeclineInvitationData.room_id = response.data.inviteFriendResponse.room_id;
        message.data.acceptOrDeclineInvitationData.invited_user_id = response.data.inviteFriendResponse.user_id;
        message.data.acceptOrDeclineInvitationData.total_time = response.data.inviteFriendResponse.total_time;
        send(client_socket, &message, sizeof(message), 0);
        // get response
        Response response;
        int bytes_received = recv(client_socket, &response, sizeof(response), 0);
        if (bytes_received <= 0)
        {
            printf("Connection closed\n");
        }
        else
        {
            printf("Received: %d bytes\n", bytes_received);
        }
        switch (response.type)
        {
        case START_GAME:
            printf("Start game\n");
            printf("White user id: %d\n", response.data.startGameData.white_user_id);
            printf("Black user id: %d\n", response.data.startGameData.black_user_id);
            printf("Room id: %d\n", response.data.startGameData.room_id);
            printf("Total time: %d\n", response.data.startGameData.total_time);
            break;
        case MOVE:
            printf("Move\n");
            printf("From x: %f\n", response.data.move.from_x);
            printf("From y: %f\n", response.data.move.from_y);
            printf("To x: %f\n", response.data.move.to_x);
            printf("To y: %f\n", response.data.move.to_y);
            printf("Piece type: %d\n", response.data.move.piece_type);
            printf("Current time: %d\n", response.data.move.current_time);
            break;
        }
    }
}
void start_game(int client_socket)
{
    // sau khi gửi lên, nhận lại message có type  = START_GAME và data là start game data đã gửi từ trước
    // bắt đầu chơi!
    Message message;
    message.type = START_GAME;
    message.data.startGame.user_id = user_id;
    message.data.startGame.room_id = room_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case START_GAME:
        // data đã gửi từ trước tại start game data
        printf("Start game\n");

        break;
    }
}

void make_move(int client_socket)
{
    Message message;
    message.type = MOVE;
    message.data.move.user_id = user_id;
    message.data.move.room_id = room_id;
    printf("ROOM: %d\n", room_id);
    float from_x, from_y, to_x, to_y;
    int piece_type;
    printf("Enter from_x: ");
    scanf("%f", &from_x);
    printf("Enter from_y: ");
    scanf("%f", &from_y);
    printf("Enter to_x: ");
    scanf("%f", &to_x);
    printf("Enter to_y: ");
    scanf("%f", &to_y);
    printf("Enter piece type: ");
    scanf("%d", &piece_type);
    message.data.move.from_x = from_x;
    message.data.move.from_y = from_y;
    message.data.move.to_x = to_x;
    message.data.move.to_y = to_y;
    message.data.move.piece_type = piece_type;
    message.data.move.current_time = 0;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }

    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }

    switch (response.type)
    {
    case MOVE:
        printf("Move\n");
        printf("From x: %f\n", response.data.move.from_x);
        printf("From y: %f\n", response.data.move.from_y);
        printf("To x: %f\n", response.data.move.to_x);
        printf("To y: %f\n", response.data.move.to_y);
        printf("Piece type: %d\n", response.data.move.piece_type);
        printf("Current time: %d\n", response.data.move.current_time);
        break;
    }
}
void send_end_game(int client_socket)
{
    int room_id;
    int status;
    printf("Enter room id: ");
    scanf("%d", &room_id);
    printf("Enter status: ");
    scanf("%d", &status);
    Message message;
    message.type = END_GAME;
    message.data.endGameData.user_id = user_id;
    message.data.endGameData.room_id = room_id;
    message.data.endGameData.status = status;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case LOGIN_RESPONSE:
        if (response.data.loginResponse.is_success == 1)
        {
            printf("Login success\n");
            printf("User id: %d\n", response.data.loginResponse.user_id);
            printf("Elo: %d\n", response.data.loginResponse.elo);
            user_id = response.data.loginResponse.user_id;
            elo = response.data.loginResponse.elo;
        }

        break;
    }
}
void send_surrender(int client_socket)
{

    Message message;
    message.type = SURRENDER;
    message.data.surrenderData.user_id = user_id;
    message.data.surrenderData.room_id = room_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // send end game with status = 0
    Message message1;
    message1.type = END_GAME;
    message1.data.endGameData.user_id = user_id;
    message1.data.endGameData.room_id = room_id;
    message1.data.endGameData.status = 0;
    int bytes_sent1 = send(client_socket, &message1, sizeof(message1), 0);
    if (bytes_sent1 <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent1);
    }
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case LOGIN_RESPONSE:
        if (response.data.loginResponse.is_success == 1)
        {
            printf("Login success\n");
            printf("User id: %d\n", response.data.loginResponse.user_id);
            printf("Elo: %d\n", response.data.loginResponse.elo);
            user_id = response.data.loginResponse.user_id;
            elo = response.data.loginResponse.elo;
        }

        break;
    }
}
void waiting_for_surrender(int client_socket)
{
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    if (response.type == SURRENDER)
    {
        printf("User %d surrender\n", response.data.surrenderData.user_id);
        printf("Room id: %d\n", response.data.surrenderData.room_id);
        // send end game with status = 1
        Message message1;
        message1.type = END_GAME;
        message1.data.endGameData.user_id = user_id;
        message1.data.endGameData.room_id = room_id;
        message1.data.endGameData.status = 1;
        int bytes_sent1 = send(client_socket, &message1, sizeof(message1), 0);
        if (bytes_sent1 <= 0)
        {
            printf("Connection closed\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Sent: %d bytes\n", bytes_sent1);
        }
        Response response1;
        int bytes_received1 = recv(client_socket, &response1, sizeof(response1), 0);
        if (bytes_received1 <= 0)
        {
            printf("Connection closed\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Received: %d bytes\n", bytes_received1);
        }
        switch (response1.type)
        {
        case LOGIN_RESPONSE:
            if (response1.data.loginResponse.is_success == 1)
            {
                printf("Login success\n");
                printf("User id: %d\n", response1.data.loginResponse.user_id);
                printf("Elo: %d\n", response1.data.loginResponse.elo);
                user_id = response1.data.loginResponse.user_id;
                elo = response1.data.loginResponse.elo;
            }
            break;
        }
    }
}
void pause_game(int client_socket)
{
    Message message;
    message.type = PAUSE;
    message.data.pauseData.user_id = user_id;
    message.data.pauseData.room_id = room_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
}
void resume_game(int client_socket)
{
    Message message;
    message.type = RESUME;
    message.data.resumeData.user_id = user_id;
    message.data.resumeData.room_id = room_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
}
void waiting_for_pause(int client_socket)
{
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    if (response.type == PAUSE)
    {
        printf("User %d pause\n", response.data.pauseData.user_id);
        printf("Room id: %d\n", response.data.pauseData.room_id);
    }
    // wait for resume
    Response response1;
    int bytes_received1 = recv(client_socket, &response1, sizeof(response1), 0);
    if (bytes_received1 <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    if (response1.type == RESUME)
    {
        printf("User %d resume\n", response1.data.resumeData.user_id);
        printf("Room id: %d\n", response1.data.resumeData.room_id);
    }
}
void get_history(int client_socket)
{
    Message message;
    message.type = GET_HISTORY;
    message.data.getGameHistory.user_id = user_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    int number_of_history = response.data.numberOfGameHistory.number_of_game;
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    printf("Number of history: %d\n", number_of_history);
    for (int i = 0; i < number_of_history; i++)
    {
        Response *history_i = (Response *)malloc(sizeof(Response));
        bytes_received = recv(client_socket, history_i, sizeof(Response), 0);
        if (bytes_received <= 0)
        {
            printf("Connection closed\n");
            exit(EXIT_FAILURE);
        }
        printf("Room id: %d\n", history_i->data.gameHistoryResponse.room_id);
        printf("Opponent username: %s\n", history_i->data.gameHistoryResponse.opponent_name);
        printf("Opponent id: %d\n", history_i->data.gameHistoryResponse.opponent_id);
        printf("Start time: %s\n", history_i->data.gameHistoryResponse.start_time);
        printf("End time: %s\n", history_i->data.gameHistoryResponse.end_time);
        if (history_i->data.gameHistoryResponse.result == 0)
        {
            printf("Result: lose\n");
        }
        else if (history_i->data.gameHistoryResponse.result == 1)
        {
            printf("Result: win\n");
        }
        else
        {
            printf("Result: draw\n");
        }
        free(history_i);
    }
}
void send_draw(int client_socket)
{
    Message message;
    message.type = DRAW;
    message.data.drawData.user_id = user_id;
    message.data.drawData.room_id = room_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // receive response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case ACCEPT_OR_DECLINE_DRAW:
        if (response.data.acceptOrDeclineDrawData.is_accept == 1)
        {
            printf("Accept draw\n");
            // send end game with status = 2
            Message message1;
            message1.type = END_GAME;
            message1.data.endGameData.user_id = user_id;
            message1.data.endGameData.room_id = room_id;
            message1.data.endGameData.status = 2;
            int bytes_sent1 = send(client_socket, &message1, sizeof(message1), 0);
            if (bytes_sent1 <= 0)
            {
                printf("Connection closed\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Sent: %d bytes\n", bytes_sent1);
            }
            // receive response
            Response response2;
            int bytes_received2 = recv(client_socket, &response2, sizeof(response2), 0);
            if (bytes_received2 <= 0)
            {
                printf("Connection closed\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Received: %d bytes\n", bytes_received2);
            }
            switch (response2.type)
            {
            case LOGIN_RESPONSE:
                if (response2.data.loginResponse.is_success == 1)
                {
                    printf("Login success\n");
                    printf("User id: %d\n", response2.data.loginResponse.user_id);
                    printf("Elo: %d\n", response2.data.loginResponse.elo);
                    user_id = response2.data.loginResponse.user_id;
                    elo = response2.data.loginResponse.elo;
                }
                break;
            }
        }
        else
        {
            printf("Decline draw\n");
        }

        break;
    }
}
void get_move_history(int client_socket)
{
    Message message;
    message.type = GET_MOVE_HISTORY;
    message.data.getMoveHistory.user_id = user_id;
    printf("Enter room id: ");
    scanf("%d", &message.data.getMoveHistory.room_id);
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    Response response;
    while (1)
    {
        int bytes_received = recv(client_socket, &response, sizeof(response), 0);
        if (bytes_received <= 0)
        {
            printf("Connection closed\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Received: %d bytes\n", bytes_received);
        }
        if (response.type == MOVE_HISTORY_RESPONSE)
        {
            printf("Room id: %d\n", response.data.moveHistory.room_id);
            printf("Move id: %d\n", response.data.moveHistory.move_id);
            printf("From x: %f\n", response.data.moveHistory.from_x);
            printf("From y: %f\n", response.data.moveHistory.from_y);
            printf("To x: %f\n", response.data.moveHistory.to_x);
            printf("To y: %f\n", response.data.moveHistory.to_y);
            printf("Piece type: %d\n", response.data.moveHistory.piece_type);

        }
        else
        {
            break;
        }
    }
}
void waiting_for_draw(int client_socket)
{
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    if (response.type == DRAW)
    {
        printf("User %d draw\n", response.data.drawData.user_id);
        printf("Room id: %d\n", response.data.drawData.room_id);
        // send end game with status = 2
        Message message1;
        message1.type = ACCEPT_OR_DECLINE_DRAW;
        message1.data.acceptOrDeclineDrawData.user_id = user_id;
        message1.data.acceptOrDeclineDrawData.room_id = room_id;
        int choice;
        printf("Enter 1 to accept, 0 to decline: ");
        scanf("%d", &choice);
        message1.data.acceptOrDeclineDrawData.is_accept = choice;
        int bytes_sent1 = send(client_socket, &message1, sizeof(message1), 0);
        if (bytes_sent1 <= 0)
        {
            printf("Connection closed\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Sent: %d bytes\n", bytes_sent1);
        }
        if (choice == 1)
        {
            // send end game
            Message message2;
            message2.type = END_GAME;
            message2.data.endGameData.user_id = user_id;
            message2.data.endGameData.room_id = room_id;
            message2.data.endGameData.status = 2;
            int bytes_sent2 = send(client_socket, &message2, sizeof(message2), 0);
            if (bytes_sent2 <= 0)
            {
                printf("Connection closed\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Sent: %d bytes\n", bytes_sent2);
            }
            // receive response
            Response response2;
            int bytes_received2 = recv(client_socket, &response2, sizeof(response2), 0);
            if (bytes_received2 <= 0)
            {
                printf("Connection closed\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Received: %d bytes\n", bytes_received2);
            }
            switch (response2.type)
            {
            case LOGIN_RESPONSE:
                if (response2.data.loginResponse.is_success == 1)
                {
                    printf("Login success\n");
                    printf("User id: %d\n", response2.data.loginResponse.user_id);
                    printf("Elo: %d\n", response2.data.loginResponse.elo);
                    user_id = response2.data.loginResponse.user_id;
                    elo = response2.data.loginResponse.elo;
                }
                break;
            }
        }
    }
}

void send_replay(int client_socket)
{
    int opponent_id;
    printf("Enter opponent id: ");
    scanf("%d", &opponent_id);
    Message message;
    message.type = REPLAY;
    message.data.replayData.user_id = user_id;
    message.data.replayData.opponent_id = opponent_id;
    send(client_socket, &message, sizeof(message), 0);
    // get response
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    switch (response.type)
    {
    case ACCEPT_REPLAY:
        if (response.data.acceptReplayData.is_accept == 1)
        {
            printf("Accept replay\n");
            // receive start game
            Response response1;
            int bytes_received1 = recv(client_socket, &response1, sizeof(response1), 0);
            if (bytes_received1 <= 0)
            {
                printf("Connection closed\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Received: %d bytes\n", bytes_received1);
            }
            switch (response1.type)
            {
            case START_GAME:
                printf("Start game\n");
                printf("White user id: %d\n", response1.data.startGameData.white_user_id);
                printf("Black user id: %d\n", response1.data.startGameData.black_user_id);
                printf("Room id: %d\n", response1.data.startGameData.room_id);
                printf("Total time: %d\n", response1.data.startGameData.total_time);
                break;
            }
        }
        break;
    }
}
void waiting_for_replay(int client_socket)
{
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    if (response.type == REPLAY)
    {
        printf("User %d want to replay\n", response.data.replayData.user_id);
        printf("Opponent id: %d\n", response.data.replayData.opponent_id);
        int choice;
        printf("Enter 1 to accept, 0 to decline: ");
        scanf("%d", &choice);
        Message message;
        message.type = ACCEPT_REPLAY;
        message.data.acceptReplayData.user_id = user_id;
        message.data.acceptReplayData.opponent_id = response.data.replayData.user_id;
        message.data.acceptReplayData.is_accept = choice;
        send(client_socket, &message, sizeof(message), 0);
        if (choice == 1)
        {
            // receive start game
            Response response1;
            int bytes_received1 = recv(client_socket, &response1, sizeof(response1), 0);
            if (bytes_received1 <= 0)
            {
                printf("Connection closed\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Received: %d bytes\n", bytes_received1);
            }
            switch (response1.type)
            {
            case START_GAME:
                printf("Start game\n");
                printf("White user id: %d\n", response1.data.startGameData.white_user_id);
                printf("Black user id: %d\n", response1.data.startGameData.black_user_id);
                printf("Black username: %s\n", response1.data.startGameData.black_username);
                printf("White username: %s\n", response1.data.startGameData.white_username);
                printf("Room id: %d\n", response1.data.startGameData.room_id);
                printf("Total time: %d\n", response1.data.startGameData.total_time);

                break;
            }
        }
    }
}
void get_top_player(int client_socket)
{
    Message message;
    message.type = GET_TOP_PLAYER;
    message.data.getTopPlayerData.user_id = user_id;
    int bytes_sent = send(client_socket, &message, sizeof(message), 0);
    if (bytes_sent <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent: %d bytes\n", bytes_sent);
    }
    // get response
}
void chat(int client_socket)
{
    Message message;
    message.type = CHAT;
    message.data.chatData.user_id = user_id;
    int friend_id;
    printf("Enter friend id: ");
    scanf("%d", &friend_id);
    message.data.chatData.friend_id = friend_id;
    char message_content[100];
    printf("Enter message: ");
    fgets(message_content, 100, stdin);
    message_content[strlen(message_content) - 1] = '\0';
    strcpy(message.data.chatData.message, message_content);
    send(client_socket, &message, sizeof(message), 0);
    getchar();
}
void get_message(int client_socket)
{
    Response response;
    int bytes_received = recv(client_socket, &response, sizeof(response), 0);
    if (bytes_received <= 0)
    {
        printf("Connection closed\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Received: %d bytes\n", bytes_received);
    }
    if (response.type == CHAT)
    {
        printf("User %d send message: %s\n", response.data.chatData.user_id, response.data.chatData.message);
    }
}
int main()
{
    int client_socket;
    struct sockaddr_in server_address;

    // Create socket
    check(client_socket = socket(AF_INET, SOCK_STREAM, 0));

    // Connect to server
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    check(connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)));
    while (1)
    {
        int choice;
        printf("TEST CLIENT\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Add Friend\n");
        printf("4. Get Online Friends\n");
        printf("5. Create Room\n");
        printf("6. Finding match\n");
        printf("7. Invite friend\n");
        printf("8. Start game\n");
        printf("9. Move\n");
        printf("10. Logout\n");
        printf("11. Waiting for invite\n");
        printf("12. End game\n");
        printf("13. Surrender\n");
        printf("14. Waiting for Surrender\n");
        printf("15. Pause\n");
        printf("16. Resume\n");
        printf("17. Waiting for pause\n");
        printf("18. Get history\n");
        printf("19. Draw\n");
        printf("20. Waiting for draw\n");
        printf("21. Replay\n");
        printf("22. Waiting for replay\n");
        printf("23. Get top player\n");
        printf("24. Chat\n");
        printf("25. Get message\n");
        printf("26. Get Move\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            login(client_socket);
            break;
        case 2:
            sendRegister(client_socket);
            break;
        case 3:
            addFriend(client_socket);
            break;
        case 4:
            get_list_online_user(client_socket);
            break;
        case 5:
            create_room(client_socket);
            break;
        case 6:
            finding_match(client_socket);
            break;
        case 7:
            invite_friend(client_socket);
            break;
        case 8:
            start_game(client_socket);
            break;
        case 9:
            make_move(client_socket);
            break;
        case 10:
            logout(client_socket);
            exit(EXIT_SUCCESS);
        case 11:
            waiting_for_invite(client_socket);
            break;
        case 12:
            send_end_game(client_socket);
            break;
        case 13:
            send_surrender(client_socket);
            break;
        case 14:
            waiting_for_surrender(client_socket);
            break;
        case 15:
            pause_game(client_socket);
            break;
        case 16:
            resume_game(client_socket);
            break;
        case 17:
            waiting_for_pause(client_socket);
            break;
        case 18:
            get_history(client_socket);
            break;
        case 19:
            send_draw(client_socket);
            break;
        case 20:
            waiting_for_draw(client_socket);
            break;
        case 21:
            send_replay(client_socket);
            break;
        case 22:
            waiting_for_replay(client_socket);
            break;
        case 23:
            get_top_player(client_socket);
            break;
        case 24:
            chat(client_socket);
            break;
        case 25:
            get_message(client_socket);
            break;
        case 26:
            get_move_history(client_socket);
            break;
        default:
            printf("Invalid choice\n");
            break;
        }
    }
}