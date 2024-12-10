/*
 * Copyright (c) 2024, <>
 */

#ifndef __HASHTABLE__
#define __HASHTABLE__

#include "linked_list.h"

typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct hashtable hashtable;
struct hashtable {
	linked_list **buckets;
	int size;
	int hmax;
	unsigned int (*hash_function)(void*);
	int (*compare_function)(void*, void*);
	void (*key_val_free_function)(void*);
};

int compare_function_ints(void *a, void *b);

int compare_function_strings(void *a, void *b);

void key_val_free_function(void *data);

hashtable *ht_create(int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*));

int ht_has_key(hashtable *ht, void *key);

void *ht_get(hashtable *ht, void *key);

void ht_put(hashtable *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);

void ht_remove_entry(hashtable *ht, void *key);

void ht_free(hashtable **ht);

#endif
