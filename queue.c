#include <stdlib.h>
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