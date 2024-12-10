/*
 * Copyright (c) 2024, <>
 */

#ifndef __LINKED_LIST__
#define __LINKED_LIST__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ll_node ll_node;
typedef struct linked_list linked_list;

struct ll_node {
    void *data;
    ll_node *next;
};

struct linked_list {
    ll_node *head;
    int size;
    int data_size;
    void (*destructor)(void *);
};

linked_list *ll_create(int data_size, void (*destructor)(void *));

ll_node *ll_node_create(int data_size, void *data);

ll_node *ll_get_nth_node(linked_list *list, int n);

int ll_insert_nth(linked_list *list, ll_node *new_node, int n);

void ll_create_and_insert(linked_list *list, int n, void *data);

ll_node *ll_remove_nth(linked_list *list, int n);

void ll_node_free(linked_list *list, ll_node **p_node);

void ll_free(linked_list **list);

void ll_print(linked_list *list, void (*print_func)(void *));

#endif
