#include <stdlib.h>
#include <stdio.h>
#include "node.h"
// Function to create a new node
node_t *create_new_node(int socket_client)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->socket_client = socket_client;
    new_node->next = NULL;
    return new_node;
}

// Function to add a node to the end of the queue
void enqueue(queue_t *list, int socket_client)
{
    node_t *new_node = create_new_node(socket_client);
    if (list->head == NULL)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        list->tail->next = new_node;
        list->tail = new_node;
    }
}

// Function to remove a node from the beginning of the queue
int dequeue(queue_t *list)
{
    if (list->head == NULL)
    {
        return -1;
    }
    else
    {
        node_t *temp = list->head;
        int socket_client = temp->socket_client;
        list->head = list->head->next;
        free(temp);
        return socket_client;
    }
}

queue_t *create_queue()
{
    queue_t *list = (queue_t *)malloc(sizeof(queue_t));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

task_t *create_new_task(int client_socket, Message message)
{
    task_t *new_task = (task_t *)malloc(sizeof(task_t));
    new_task->client_socket = client_socket;
    new_task->message = message;
    new_task->next = NULL;
    return new_task;
}

void enqueue_task(task_queue_t *list, int client_socket, Message message)
{
    task_t *new_task = create_new_task(client_socket, message);
    // printf("added task to queue for socket: %d\n", client_socket);
    if (list->head == NULL)
    {
        list->head = new_task;
        list->tail = new_task;
    }
    else
    {
        list->tail->next = new_task;
        list->tail = new_task;
    }
}

task_t *dequeue_task(task_queue_t *list)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    else
    {
        task_t *temp = list->head;
        list->head = list->head->next;
        return temp;
    }
}

task_queue_t *create_task_queue()
{
    task_queue_t *list = (task_queue_t *)malloc(sizeof(task_queue_t));
    list->head = NULL;
    list->tail = NULL;
    return list;
}