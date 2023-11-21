
typedef struct node
{
    int client_socket;
    struct node *next;
} node_t;

node_t *head;
node_t *tail;
node_t *front();