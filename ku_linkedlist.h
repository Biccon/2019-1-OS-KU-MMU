#include <stdio.h>
#include <stdlib.h>

typedef struct ku_h_pcb
{
    char pid;             // process pid
    struct ku_pte **pdba; // page directory base address
} ku_h_pcb;

typedef struct ku_h_node
{
    ku_h_pcb *pcb;
    struct ku_h_node *prev;
    struct ku_h_node *next;
} ku_h_node;

typedef struct ku_h_linkedlist
{
    ku_h_node *header;
    ku_h_node *tailer;
} ku_h_linkedlist;

ku_h_pcb *ku_h_make_pcb(char pid)
{
    ku_h_pcb *new_pcb = (ku_h_pcb *)malloc(sizeof(ku_h_pcb));
    new_pcb->pid = pid;
    new_pcb->pdba = NULL;
    return new_pcb;
}
int ku_h_get_size(ku_h_linkedlist *list)
{
    int size = 0;
    ku_h_node *temp = list->header->next;
    while (temp != list->tailer)
    {
        size++;
        temp = temp->next;
    }
    return size;
}

ku_h_node *ku_h_add_after(ku_h_node *node, char pid)
{
    ku_h_node *new_node = (ku_h_node *)malloc(sizeof(ku_h_node));
    new_node->pcb = ku_h_make_pcb(pid);
    new_node->prev = node;
    new_node->next = node->next;

    node->next->prev = new_node;
    node->next = new_node;
    return new_node;
}

ku_h_node *ku_h_add_before(ku_h_node *node, char pid)
{
    ku_h_node *new_node = (ku_h_node *)malloc(sizeof(ku_h_node));
    new_node->pcb = ku_h_make_pcb(pid);
    new_node->prev = node->prev;
    new_node->next = node;

    node->prev->next = new_node;
    node->prev = new_node;
    return new_node;
}

ku_h_node *ku_h_add_first(ku_h_linkedlist *list, char pid)
{
    return ku_h_add_after(list->header, pid);
}

ku_h_node *ku_h_add_last(ku_h_linkedlist *list, char pid)
{
    return ku_h_add_before(list->tailer, pid);
}

ku_h_node *ku_h_get_node_by_index(ku_h_linkedlist *list, int index)
{
    ku_h_node *temp = list->header->next;
    for (int i = 0; (i < index && temp != list->tailer); i++)
    {
        temp = temp->next;
    }
    return temp;
}

ku_h_node *ku_h_get_node_by_pid(ku_h_linkedlist *list, char pid)
{
    ku_h_node *temp = list->header->next;
    while (temp != list->tailer)
    {
        if (temp->pcb->pid == pid)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

ku_h_node *ku_h_get_first(ku_h_linkedlist *list)
{
    return ku_h_get_node_by_index(list, 0);
}

ku_h_node *ku_h_get_last(ku_h_linkedlist *list)
{
    return ku_h_get_node_by_index(list, ku_h_get_size(list) - 1);
}

void print_bit(unsigned char a)
{
    unsigned char filter = 0x80;
    for (int i = 0; i < 8; i++)
    {
        printf("%d", (a & filter) != 0);
        filter >>= 1;
    }
    printf("\n");
}

void ku_h_print_list(ku_h_linkedlist *list)
{
    printf("process list\n");
    ku_h_node *temp = list->header->next;
    while (temp != list->tailer)
    {
        printf("%d\n", temp->pcb->pid);
        temp = temp->next;
    }
    printf("--------------\n");
}

void ku_h_init_list(ku_h_linkedlist *list)
{
    list->header = (ku_h_node *)malloc(sizeof(ku_h_node));
    list->tailer = (ku_h_node *)malloc(sizeof(ku_h_node));

    list->header->prev = NULL;
    list->header->next = list->tailer;

    list->tailer->prev = list->header;
    list->tailer->next = NULL;
}