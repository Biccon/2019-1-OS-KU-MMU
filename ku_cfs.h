#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Node Node;
typedef struct _LinkedList LinkedList;
struct _Node
{
    Node *prev_node;
    Node *next_node;
    int swapable;
    int data;
};
struct _LinkedList
{
    Node *head;
    Node *tail;
};

void init_list(LinkedList *list)
{
    list->head = (Node *)malloc(sizeof(Node));
    list->tail = (Node *)malloc(sizeof(Node));

    list->head->prev_node = NULL;
    list->head->next_node = list->tail;
    list->tail->prev_node = list->head;
    list->tail->next_node = NULL;
}

Node *get_first(LinkedList *list)
{
    return list->head->next_node;
}

Node *insert_next(Node *node, int data, int swapable)
{
    Node *new_node = (Node *)malloc(sizeof(Node));

    new_node->prev_node = node;
    new_node->next_node = node->next_node;
    new_node->data = data;
    new_node->swapable = swapable;
    if ((node->next_node) != NULL)
        node->next_node->prev_node = new_node;
    node->next_node = new_node;

    return new_node;
}

Node *insert_last(LinkedList *list, int data, int swapable)
{
    return insert_next(list->tail->prev_node, data, swapable);
}

void change_node(Node *prev, Node *next)
{
    int temp_data = prev->data;
    prev->data = next->data;
    next->data = temp_data;

    int temp_swapable = prev->swapable;
    prev->swapable = next->swapable;
    next->swapable = temp_swapable;
}

void free_list(LinkedList *list)
{
    Node *temp = list->head->next_node;
    while (temp != (list->tail))
    {
        Node *temp2 = temp;
        temp = temp->next_node;
        free(temp2);
    }
    free(list->head);
    free(list->tail);
    free(list);
}

int get_swapable_page(LinkedList *list){
    Node *temp = list->head->next_node;
    while(temp != (list->tail)){
        
        temp = temp -> next_node;
    }
}