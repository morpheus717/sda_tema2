/*
 * Copyright (c) 2024, <>
 */

#include "doubly_linked_list.h"

d_linked_list *dll_create(int data_size, void (*destructor)(void *))
{
    d_linked_list *list = malloc(sizeof(d_linked_list));
    list->data_size = data_size;
    list->destructor = destructor;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

dll_node *dll_node_create(int data_size, void *data)
{
    dll_node *node = malloc(sizeof(dll_node));
    node->data = malloc(data_size);
    node->prev = NULL;
    node->next = NULL;
    memcpy(node->data, data, data_size);
    return node;
}

dll_node *dll_get_nth_node(d_linked_list *list, int n)
{
    if (!list || !list->head) {
        printf("lista goala\n");
        return NULL;
    }
    if (n > list->size) {
        printf("prea mare\n");
        return NULL;
    }
    if (n <= list->size - n) {
        dll_node *curr;
        for (curr = list->head; curr && n; curr = curr->next, n--)
            continue;
        return curr;
    } else {
        dll_node *curr;
        for (curr = list->tail; curr && n < list->size - 1;
             curr = curr->prev, n++)
            continue;
        return curr;
    }
}

int dll_search_by_addr(d_linked_list *list, dll_node *node)
{
    dll_node *curr = list->head;
    while (curr) {
        if (curr == node)
            return 1;
        curr = curr->next;
    }
    return 0;
}

int dll_insert_nth(d_linked_list *list, dll_node *node, int n)
{
    if (!list)
        return 0;
    if (n > list->size) {
        printf("dimensiune lista depasita\n");
        return 0;
    }
    if (!list->head) {
        list->head = node;
        list->tail = node;
        list->size++;
        return 1;
    }
    if (n == 0) {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
        list->size++;
        return 1;
    }
    if (n == list->size) {
        node->next = NULL;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
        list->size++;
        return 1;
    }
    dll_node *tmp = dll_get_nth_node(list, n);
    node->prev = tmp->prev;
    node->next = tmp;
    tmp->prev->next = node;
    tmp->prev = node;
    list->size++;
    return 1;
}

dll_node *dll_create_and_insert(d_linked_list *list, int n, void *data)
{
    dll_node *nod = dll_node_create(list->data_size, data);
    if (dll_insert_nth(list, nod, n) == 0) {
        dll_node_free(list, &nod);
        nod = NULL;
    }
    return nod;
}

dll_node *dll_remove_by_addr(d_linked_list *list, dll_node *node)
{
    list->size--;
    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;
    return node;
}

dll_node *dll_remove_nth(d_linked_list *list, int n)
{
    if (n >= list->size) {
        printf("dimensiune lista depasita\n");
        return NULL;
    }
    if (!list || !list->head) {
        printf("lista nealocata sau goala\n");
        return NULL;
    }
    dll_node *eliminated = dll_get_nth_node(list, n);
    return dll_remove_by_addr(list, eliminated);
}

void dll_node_free(d_linked_list *list, dll_node **p_node)
{
    list->destructor((*p_node)->data);
    free(*p_node);
}

void dll_free(d_linked_list **p_list)
{
    for (dll_node *curr = (*p_list)->head; curr; ) {
        dll_node *freed = curr;
        curr = curr->next;
        dll_node_free(*p_list, &freed);
    }
    free(*p_list);
}

void dll_print(d_linked_list *list, void (*print_func)(void *))
{
    for (dll_node *curr = list->head; curr; curr = curr->next)
        print_func(curr->data);
    printf("\n");
}

dll_node *dll_pop_front(d_linked_list *list)
{
    dll_node *elim = dll_remove_nth(list, 0);
    return elim;
}

dll_node *dll_pop_back(d_linked_list *list)
{
    dll_node *elim = dll_remove_nth(list, list->size - 1);
    return elim;
}

void dll_add_front(d_linked_list *list, void *data)
{
    dll_create_and_insert(list, 0, data);
}

void dll_add_back(d_linked_list *list, dll_node *nod)
{
    dll_insert_nth(list, nod, list->size);
}
