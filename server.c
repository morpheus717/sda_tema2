/*
 * Copyright (c) 2024, <>
 */

#include <stdio.h>
#include "server.h"
#include "lru_cache.h"

#include "utils.h"

void request_destroy(void *data)
{
    request *req = (request *)data;
    free(req->doc_content);
    free(req->doc_name);
    free(data);
}
void init_resp(response **rsp)
{
    *rsp = malloc(sizeof(response));
    (*rsp)->server_log = malloc(sizeof(char) * MAX_LOG_LENGTH);
    (*rsp)->server_response = malloc(sizeof(char) * MAX_RESPONSE_LENGTH);
}
request *init_request()
{
    request *req = malloc(sizeof(request));
    req->doc_name = malloc(DOC_NAME_LENGTH);
    req->doc_content = malloc(DOC_CONTENT_LENGTH);
    return req;
}
void copy_request(request *dest, request *src)
{
    dest->type = src->type;
    memcpy(dest->doc_name, src->doc_name, strlen(src->doc_name) + 1);
    memcpy(dest->doc_content, src->doc_content, strlen(src->doc_content) + 1);
}
void print_resp(void *data)
{
    response rsp = *(response *)data;
    printf("%s %s %d\n", rsp.server_log, rsp.server_response, rsp.server_id);
}
void print_req(void *data)
{
    request rsp = *(request *)data;
    printf("%d %s %s\n", rsp.type, rsp.doc_name, rsp.doc_content);
}

static response
*server_edit_document(server *s, char *doc_name, char *doc_content) {
    response *rsp;
    init_resp(&rsp);
    rsp->server_id = s->id;
    void *evicted = NULL;
    if (lru_cache_put(s->cache, doc_name, doc_content, &evicted) == 0) {
        // exista in cache
        sprintf(rsp->server_response, MSG_B, doc_name);
        sprintf(rsp->server_log, LOG_HIT, doc_name);
    } else {
        // exista in database
        if (ht_get(s->local_db, doc_name))
            sprintf(rsp->server_response, MSG_B, doc_name);
        // nu exista in database
        else
            sprintf(rsp->server_response, MSG_C, doc_name);
        // cacheul nu a fost plin
        if (evicted == NULL) {
            sprintf(rsp->server_log, LOG_MISS, doc_name);
        } else {            // cacheul a fost plin
            char *c_evc = (char *)evicted;
            sprintf(rsp->server_log, LOG_EVICT, doc_name, c_evc);
            free(evicted);
        }
    }       // updatam baza de date
    ht_put(s->local_db, doc_name, sizeof(char) * ((strlen(doc_name) + 1)),
        doc_content, sizeof(char) * (strlen(doc_content) + 1));
    return rsp;
}

static response
*server_get_document(server *s, char *doc_name) {
    response *rsp;
    init_resp(&rsp);
    rsp->server_id = s->id;
    char *content = lru_cache_get(s->cache, doc_name);
    // daca exista in cache
    if (content) {
        sprintf(rsp->server_log, LOG_HIT, doc_name);
        memcpy(rsp->server_response, content, sizeof(char) *
        (strlen(content) + 1));
        void *dummy;
        lru_cache_put(s->cache, doc_name, content, &dummy);
        return rsp;
    }
    // nu a fost in cache, poate in database
    char *db_content = ht_get(s->local_db, doc_name);
    if (!db_content) {
        free(rsp->server_response);
        rsp->server_response = NULL;
        sprintf(rsp->server_log, LOG_FAULT, doc_name);
        return rsp;
    }
    memcpy(rsp->server_response, db_content, sizeof(char) *
    (strlen(db_content) + 1));
    void *evicted;      // updatam cacheul
    lru_cache_put(s->cache, doc_name, db_content, &evicted);
    if (!evicted) {
        sprintf(rsp->server_log, LOG_MISS, doc_name);
    } else {
        char *c_evc = (char *)evicted;
        sprintf(rsp->server_log, LOG_EVICT, doc_name, c_evc);
        free(evicted);
    }
    return rsp;
}

server *init_server(unsigned int cache_size, int server_id) {
    server *s = malloc(sizeof(server));
    s->cache = init_lru_cache(cache_size);
    s->local_db = ht_create(HT_SIZE, hash_string, compare_function_strings,
    key_val_free_function);
    s->task_q = dll_create(sizeof(request), request_destroy);
    s->id = server_id;
    return s;
}

response *server_handle_request(server *s, request *req) {
    if (req->type == 0) {       // edit
        request *aux = init_request();
        copy_request(aux, req);
        dll_add_front(s->task_q, aux);
        free(aux);
        response *resp;
        init_resp(&resp);
        resp->server_id = s->id;
        sprintf(resp->server_response, MSG_A, "EDIT", req->doc_name);
        sprintf(resp->server_log, LOG_LAZY_EXEC, s->task_q->size);
        return resp;
    } else if (req->type == 1) {        // get
        while (s->task_q->size > 0) {
            dll_node *edit = dll_pop_back(s->task_q);
            request *edit2 = (request *)edit->data;
            response *local;
            local = server_edit_document(s, edit2->doc_name,
            edit2->doc_content);
            PRINT_RESPONSE(local);
            dll_node_free(s->task_q, &edit);
        }
        return server_get_document(s, req->doc_name);
    }
    return NULL;
}

void free_server(server **s) {
    free_lru_cache(&(*s)->cache);
    ht_free(&(*s)->local_db);
    dll_free(&(*s)->task_q);
    free(*s);
}
