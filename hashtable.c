/*
 * Copyright (c) 2024, <>
 */

#include "hashtable.h"

int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;
	return strcmp(str_a, str_b);
}

void key_val_free_function(void *data)
{
	info *be_free = (info*)data;
	free(be_free->key);
	free(be_free->value);
	free(data);
}

hashtable *ht_create(int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*))
{
	hashtable *ht = malloc(sizeof(hashtable));
	ht->hmax = hmax;
	ht->hash_function = (*hash_function);
	ht->compare_function = (*compare_function);
	ht->key_val_free_function = (*key_val_free_function);
	ht->size = 0;
	ht->buckets = malloc(hmax * sizeof(linked_list *));
	for (int i = 0; i < hmax; i++)
		ht->buckets[i] = ll_create(sizeof(info), ht->key_val_free_function);
	return ht;
}

int ht_has_key(hashtable *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_node *current = ht->buckets[index]->head;
	while (current != NULL) {
		if (ht->compare_function(((info*)(current->data))->key, key) == 0)
			return 1;
		current = current->next;
	}
	return 0;
}

void *ht_get(hashtable *ht, void *key)
{
	if (ht_has_key(ht, key) == 0)
		return NULL;
	int index = ht->hash_function(key) % ht->hmax;
	ll_node *current = ht->buckets[index]->head;
	while (current != NULL) {
		info *data = (info*)current->data;
		if (ht->compare_function(data->key, key) == 0)
			return data->value;
		current = current->next;
	}
	return NULL;
}

void ht_put(hashtable *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	if (ht_has_key(ht, key))
		ht_remove_entry(ht, key);
	int index = ht->hash_function(key) % ht->hmax;
	info *new_info = malloc(sizeof(info));
	new_info->key = malloc(key_size);
	new_info->value = malloc(value_size);
	memcpy(new_info->key, key, key_size);
	memcpy(new_info->value, value, value_size);
	ll_create_and_insert(ht->buckets[index],
	ht->buckets[index]->size, new_info);
	free(new_info);
}

void ht_remove_entry(hashtable *ht, void *key)
{
	if (ht_has_key(ht, key) == 0)
		return;
	int index = ht->hash_function(key) % ht->hmax;
	ll_node *current = ht->buckets[index]->head;
	int i = 0;
	while (current != NULL) {
		info *data = (info*)current->data;
		if (ht->compare_function(data->key, key) == 0) {
			ll_node *eliminated = ll_remove_nth(ht->buckets[index], i);
			ht->key_val_free_function(eliminated->data);
			free(eliminated);
			return;
		}
		current = current->next;
		i++;
	}
}

void ht_remove_entry_2(hashtable *ht, void *key,
					   void (*key_val_free_func)(void *))
{
	if (ht_has_key(ht, key) == 0)
		return;
	int index = ht->hash_function(key) % ht->hmax;
	ll_node *current = ht->buckets[index]->head;
	int i = 0;
	while (current != NULL) {
		info *data = (info*)current->data;
		if (ht->compare_function(data->key, key) == 0) {
			ll_node *eliminated = ll_remove_nth(ht->buckets[index], i);
			key_val_free_func(eliminated->data);
			free(eliminated);
			return;
		}
		current = current->next;
		i++;
	}
}

void ht_free(hashtable **ht)
{
    for (int i = 0; i < (*ht)->hmax; i++) {
		if ((*ht)->buckets[i] != NULL)
		ll_free(&(*ht)->buckets[i]);
	}
	free((*ht)->buckets);
	free(*ht);
}
