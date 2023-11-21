#ifndef NODE_H
#define NODE_H

typedef struct node
{
    int socket_client;
    struct node *next;
} node_t;

// Function declarations
node_t *create_new_node(int socket_client);
void enqueue(node_t **head, int socket_client);
int dequeue(node_t **head);

#endif