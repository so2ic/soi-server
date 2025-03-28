#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

typedef unsigned int uint;

typedef struct ll_element ll_element;

struct ll_element
{
    void* object;
    ll_element* prev;
    ll_element* next;
};

typedef struct 
{
    ll_element* head;
    ll_element* tail;
    uint count;
} ll_t;

ll_t* ll_init();
int ll_insert(ll_t* l, void* data);
void* ll_get_head(ll_t* l);
void* ll_get_tail(ll_t* l);
void* ll_get_data_at(ll_t* l, int place);
int ll_remove_at(ll_t* l, int place);
int ll_insert_at(ll_t* l, void* data, int place);
int ll_get_size(ll_t* l);
int ll_copy(ll_t* dest, ll_t* src);

#endif // LINKED_LIST_H

