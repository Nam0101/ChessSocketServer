#ifndef NODE_H
#define NODE_H

typedef struct node
{
    int socket_client;
    struct node *next;
} node_t;

typedef struct queue
{
    node_t *head;
    node_t *tail;
} queue_t;
// Function declarations
node_t *create_new_node(int socket_client);
void enqueue(queue_t *list, int socket_client);
int dequeue(queue_t *list);
queue_t *create_queue();
#endif