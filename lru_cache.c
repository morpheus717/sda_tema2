/*
 * Copyright (c) 2024, <>
 */

#include <stdio.h>
#include <string.h>
#include "lru_cache.h"
#include "utils.h"

void document_destroyer(void *doc)
{
    document *d = (document *)doc;
    free(d->content);
    free(d->name);
    free(doc);
}

void print_document(void *doc)
{
    document *d = (document *)doc;
    printf("%s %s\n", d->name, d->content);
}

lru_cache *init_lru_cache(unsigned int cache_capacity) {
    lru_cache *cache = malloc(sizeof(lru_cache));
    cache->capacity = cache_capacity;
    cache->dll = dll_create(sizeof(document), document_destroyer);
    cache->ht = ht_create(cache_capacity, hash_string,
    compare_function_strings, key_val_free_function);
    return cache;
}

bool lru_cache_is_full(lru_cache *cache) {
    if (cache->dll->size == cache->capacity)
        return true;
    return false;
}

void free_lru_cache(lru_cache **cache) {
    dll_free(&(*cache)->dll);
    ht_free(&(*cache)->ht);
    free(*cache);
}

void lru_prioritize(lru_cache *cache, dll_node *node)
{
    dll_remove_by_addr(cache->dll, node);
    dll_add_back(cache->dll, node);
}

document *document_creator(void *name, void *content)
{
    document *doc = malloc(sizeof(document));
    doc->content = malloc(strlen(content) + 1);
    memcpy(doc->content, content, strlen(content) + 1);
    doc->name = malloc(strlen(name) + 1);
    memcpy(doc->name, name, strlen(name) + 1);
    return doc;
}

void lru_push(lru_cache *cache, void *key, void *value)
{
    document *d = document_creator(key, value);
    dll_node *new = dll_node_create(cache->dll->data_size, d);
    ht_put(cache->ht, key, strlen(key) + 1, &new, sizeof(dll_node **));
    dll_insert_nth(cache->dll, new, cache->dll->size);
    free(d);
}

void lru_delete(lru_cache *cache, void *key)
{
    dll_node *found = *(dll_node **)ht_get(cache->ht, key);
    dll_remove_by_addr(cache->dll, found);
    dll_node_free(cache->dll, &found);
    found = NULL;
    ht_remove_entry(cache->ht, key);
}

void lru_pop_first(lru_cache *cache)
{
    dll_node *deleted = dll_pop_front(cache->dll);
    char *key = ((document *)deleted->data)->name;
    char *key_copy = malloc(strlen(key) + 1);
    memcpy(key_copy, key, strlen(key) + 1);
    dll_node_free(cache->dll, &deleted);
    deleted = NULL;
    ht_remove_entry(cache->ht, key_copy);
    free(key_copy);
}

bool lru_cache_put(lru_cache *cache, void *key, void *value,
                   void **evicted_key) {
    *evicted_key = NULL;
    if (ht_has_key(cache->ht, key) == 0) {
        if (lru_cache_is_full(cache)) {
            dll_node *evicted = dll_get_nth_node(cache->dll, 0);
            char *ev_key = ((document *)evicted->data)->name;
            *evicted_key = malloc(sizeof(char) * (strlen(ev_key) + 1));
            memcpy(*evicted_key, ev_key, sizeof(char) * (strlen(ev_key) + 1));
            lru_delete(cache, *evicted_key);
        }
        lru_push(cache, key, value);
        return true;
    }
    dll_node *found = *(dll_node **)ht_get(cache->ht, key);
    document *doc = (document *)found->data;
    char *aux_value = malloc(strlen(value) + 1);
    memcpy(aux_value, value, strlen(value) + 1);
    free(doc->content);
    doc->content = aux_value;
    lru_prioritize(cache, found);
    return false;
}

void *lru_cache_get(lru_cache *cache, void *key) {
    if (ht_has_key(cache->ht, key) == 0)
        return NULL;
    dll_node *nod = *(dll_node **)ht_get(cache->ht, key);
    lru_prioritize(cache, nod);
    return ((document *)nod->data)->content;
}

void lru_cache_remove(lru_cache *cache, void *key) {
    if (ht_has_key(cache->ht, key) == 0)
        return;
    dll_node *elim = *(dll_node **)ht_get(cache->ht, key);
    dll_remove_by_addr(cache->dll, elim);
    dll_node_free(cache->dll, &elim);
    elim = NULL;
    ht_remove_entry(cache->ht, key);
}
