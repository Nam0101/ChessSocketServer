#include <stdlib.h>
#include "node.h"

// Function to create a new node
node_t *create_new_node(int socket_client)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL)
    {
        exit(-1); // Failed to allocate memory
    }
    new_node->socket_client = socket_client;
    new_node->next = NULL;
    return new_node;
}

// Function to add a node to the end of the queue
void enqueue(node_t **head, int socket_client)
{
    node_t *new_node = create_new_node(socket_client);
    if (*head == NULL)
    {
        *head = new_node;
        return;
    }
    node_t *current = *head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = new_node;
}

// Function to remove a node from the front of the queue
int dequeue(node_t **head)
{
    if (*head == NULL)
    {
        return -1; // Queue is empty
    }
    node_t *next_node = (*head)->next;
    int socket_client = (*head)->socket_client;
    free(*head);
    *head = next_node;
    return socket_client;
}