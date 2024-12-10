/*
 * Copyright (c) 2024, <>
 */

#ifndef __DOUBLY_LINKED_LIST__
#define __DOUBLY_LINKED_LIST__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dll_node dll_node;
typedef struct d_linked_list d_linked_list;

struct dll_node {
    void *data;
    dll_node *prev, *next;
};
struct d_linked_list {
    dll_node *head, *tail;
    int size;
    int data_size;
    void (*destructor)(void *);
};

d_linked_list *dll_create(int data_size, void (*destructor)(void *));

dll_node *dll_node_create(int data_size, void *data);

dll_node *dll_get_nth_node(d_linked_list *list, int n);

int dll_search_by_addr(d_linked_list *list, dll_node *node);

int dll_insert_nth(d_linked_list *list, dll_node *node, int n);

dll_node *dll_remove_by_addr(d_linked_list *list, dll_node *node);

dll_node *dll_create_and_insert(d_linked_list *list, int n, void *data);

dll_node *dll_remove_nth(d_linked_list *list, int n);

void dll_node_free(d_linked_list *list, dll_node **p_node);

void dll_free(d_linked_list **p_list);

void dll_print(d_linked_list *list, void (*print_func)(void *));

dll_node *dll_pop_front(d_linked_list *list);

dll_node *dll_pop_back(d_linked_list *list);

void dll_add_front(d_linked_list *list, void *data);

void dll_add_back(d_linked_list *list, dll_node *nod);

#endif
