/*
 * Copyright (c) 2024, <>
 */

#include "linked_list.h"

linked_list *ll_create(int data_size, void (*destructor)(void *))
{
    linked_list *list = malloc(sizeof(linked_list));
    list->data_size = data_size;
    list->head = NULL;
    list->size = 0;
    list->destructor = destructor;
    return list;
}

ll_node *ll_node_create(int data_size, void *data)
{
    ll_node *new_node = malloc(sizeof(ll_node));
    new_node->data = malloc(data_size);
    memcpy(new_node->data, data, data_size);
    new_node->next = NULL;
    return new_node;
}

ll_node *ll_get_nth_node(linked_list *list, int n)
{
    if (!list || !list->head)
        return NULL;
    ll_node *cur;
    for (cur = list->head; cur && n; cur = cur->next, n--)
        continue;
    return cur;
}

int ll_insert_nth(linked_list *list, ll_node *new_node, int n)
{
    if (n > list->size) {
        printf("dimensiune lista depasita\n");
        return 0;
    }
    if (!list) {
        printf("lista nealocata\n");
        return 0;
    }
    if (!list->head) {
        list->head = new_node;
        new_node->next = NULL;
        list->size++;
        return 1;
    }
    if (n == 0) {
        new_node->next = list->head;
        list->head = new_node;
        list->size++;
        return 1;
    }
    ll_node *prev = ll_get_nth_node(list, n - 1);
    new_node->next = prev->next;
    prev->next = new_node;
    list->size++;
    return 1;
}

void ll_create_and_insert(linked_list *list, int n, void *data)
{
    ll_node *node = ll_node_create(list->data_size, data);
    if (ll_insert_nth(list, node, n) == 0)
        ll_node_free(list, &node);
}

ll_node *ll_remove_nth(linked_list *list, int n)
{
    if (n >= list->size) {
        printf("dimensiune lista depasita\n");
        return NULL;
    }
    if (!list || !list->head) {
        printf("lista nealocata sau goala\n");
        return NULL;
    }
    list->size--;
    if (n == 0) {
        ll_node *eliminated = list->head;
        list->head = eliminated->next;
        return eliminated;
    }
    ll_node *prev = ll_get_nth_node(list, n - 1);
    ll_node *eliminated = prev->next;
    prev->next = prev->next->next;
    return eliminated;
}

void ll_node_free(linked_list *list, ll_node **p_node)
{
    list->destructor((*p_node)->data);
    free(*p_node);
}

void ll_free(linked_list **p_list)
{
    if (!(*p_list))
        return;
    for (ll_node *curr = (*p_list)->head; curr; ) {
        ll_node *freed = curr;
        curr = curr->next;
        ll_node_free(*p_list, &freed);
    }
    free(*p_list);
}
void ll_print(linked_list *list, void (*print_func)(void *))
{
    for (ll_node *curr = list->head; curr; curr = curr->next)
        print_func(curr->data);
    printf("\n");
}
