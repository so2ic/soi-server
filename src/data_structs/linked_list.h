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
    size_t size;
    uint count;
} ll_t;

static ll_t* ll_init()
{
    ll_t* l = (ll_t*) malloc(sizeof(ll_t)); 
    l->size = 0; 
    l->count = 0;
    l->head = NULL;
    l->tail = NULL;

    return l;
}

static int ll_insert(ll_t* l, void* data)
{
    if(l->head == NULL)
    {
        l->head = (void*) malloc(sizeof(data)); 
        l->tail = (void*) malloc(sizeof(data));
        if(l->head == NULL)
            return 1;

        l->head->object = data;
        l->head->next = NULL;
        l->head->prev = NULL;
        l->tail = l->head;
    }
    else
    {
        ll_element* cursor = l->head;
        while(cursor->next != NULL)
            cursor = cursor->next;

        cursor->next = (void*) malloc(sizeof(data));
        if(cursor->next == NULL)
            return 1;
        
        cursor->next->prev = cursor;
        cursor->object = data;
        cursor->next->next = NULL;
        l->tail = cursor;
    }

    l->size++;
    l->count++;
    return 0;
}

static void* ll_get_head(ll_t* l)
{
    return l->head;
}

static void* ll_get_tail(ll_t* l)
{
    return l->tail;
}

static void* ll_get_data_at(ll_t* l, int place)
{
    if(place > l->count)
        return NULL;

    if(place == 0)
    {
        return l->head->object; 
    }
    else
    {
        ll_element* el = l->head;
        for(int i = 1; i < place; ++i)
        {
            el = el->next; 
        }

        return el;
    }

    return NULL;
}

static int ll_remove_at(ll_t* l, int place)
{
    if(place > l->count)
        return 1;

    if(place == 0)
    {
        if(l->head->next == NULL)
        {
            free(l->head);
            l->head = NULL;
        }
        else
        {
            ll_element* tmp;
            l->head = l->head->next;
            free(tmp);
            tmp = NULL;
        }
    }
    else if(place == l->count-1)
    {
    
    }
    else
    {
        ll_element* el = l->head;
        for(int i = 1; i < place; ++i)
        {
            el = el->next;
        }
        ll_element* tmp = el;
        el->prev->next = el->next;
        el->next->prev = el->prev;
        free(tmp);
        tmp = NULL;
    }

    l->count--;
    l->size--;
    return 0;
}

#endif // LINKED_LIST_H
