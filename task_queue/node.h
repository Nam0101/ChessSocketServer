#ifndef NODE_H
#define NODE_H
#include "../server.h"
// node for handling incomming connections
typedef struct node
{
    int socket_client;
    struct node *next;
} node_t;
// queue for handling incomming connections
typedef struct queue
{
    node_t *head;
    node_t *tail;
} queue_t;
typedef struct task
{
    int client_socket;
    Message message;
    struct task *next;
} task_t;
typedef struct task_queue
{
    task_t *head;
    task_t *tail;
} task_queue_t;

// Function declarations
node_t *create_new_node(int socket_client);
void enqueue(queue_t *list, int socket_client);
int dequeue(queue_t *list);
queue_t *create_queue();
task_t *create_new_task(int client_socket, Message message);
void enqueue_task(task_queue_t *list, int client_socket, Message message);
task_t *dequeue_task(task_queue_t *list);
task_queue_t *create_task_queue();
#endif