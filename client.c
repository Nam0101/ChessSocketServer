#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/sha.h>
#include "server.h"
#define PORT 12345
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
int user_id, elo;
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
    int friend_id;
    printf("Enter friend id: ");
    scanf("%d", &friend_id);
    message.data.addFriendData.friend_id = friend_id;
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
        printf("Number of online friends: %d\n", response.data.onlineFriendsResponse.number_of_friends);
        for (int i = 0; i < response.data.onlineFriendsResponse.number_of_friends; i++)
        {
            printf("%d ", response.data.onlineFriendsResponse.friend_id[i]);
        }
        printf("\n");
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
        }
        else
        {
            if (response.data.createRoomResponse.message_code == USER_LOGED_IN)
            {
                printf("User loged in\n");
            }
            else
            {
                printf("Server error\n");
            }
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
    case FINDING_MATCH_RESPONSE:
        if (response.data.findingMatchResponse.is_success == 1)
        {
            printf("Opponent id: %d\n", response.data.findingMatchResponse.opponent_id);
        }
        else
        {
            printf("Finding match failed\n");
        }
        break;
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
        printf("10. Logout\n");
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
        case 10:
            logout(client_socket);
            exit(EXIT_SUCCESS);
        default:
            printf("Invalid choice\n");
            break;
        }
    }
}