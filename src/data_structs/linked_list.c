#include "linked_list.h"

ll_t* ll_init()
{
    ll_t* l = (ll_t*) malloc(sizeof(ll_t)); 
    l->count = 0;
    l->head = NULL;
    l->tail = NULL;
    return l;
}

int ll_insert(ll_t* l, void* data)
{
    ll_element* new_element = malloc(sizeof(ll_element));
    if(new_element == NULL)
        return 1;

    new_element->object = data;
    new_element->next = NULL;

    if(l->head == NULL)
    {
        new_element->prev = NULL;
        l->head = new_element;
        l->tail = new_element;
    }
    else
    {
        new_element->prev = l->tail;
        l->tail->next = new_element;
        l->tail = new_element;
    }

    l->count++;
    return 0;
}

void* ll_get_head(ll_t* l)
{
    return (l->head) ? l->head->object : NULL;
}

void* ll_get_tail(ll_t* l)
{
    return (l->tail) ? l->tail->object : NULL;
}

void* ll_get_data_at(ll_t* l, int place)
{
    if(place >= (int) l->count || place < 0)
        return NULL;

    ll_element* el = l->head;
    for(int i = 0; i < place; ++i)
        el = el->next;
    
    return el->object;
}

int ll_remove_at(ll_t* l, int place)
{
    if(place >= (int) l->count || place < 0)
        return 1;

    ll_element* el = l->head;
    if(place == 0)
    {
        l->head = l->head->next;
        if(l->head == NULL)
            l->tail = NULL;
        else
            l->head->prev = NULL;
    }
    else if(place == (int) l->count-1)
    {
        el = l->tail;
        l->tail = l->tail->prev;
        l->tail->next = NULL;
    }
    else
    {
        for(int i = 0; i < place; ++i)
            el = el->next;

        el->prev->next = el->next;
        el->next->prev = el->prev;
    }

    free(el);
    l->count--;
    return 0;
}

int ll_insert_at(ll_t* l, void* data, int place)
{
    if(place >= (int) l->count || place < 0)
        return 1;

    if(place == 0)
        return ll_insert(l, data);

    ll_element* l_el = l->head;
    ll_element* el = malloc(sizeof(ll_element));
    if(el == NULL)
        return 1;

    el->object = data;

    for(int i = 0; i < place; ++i)
        l_el = l_el->next;

    el->prev = l_el;
    el->next = l_el->next;
    l_el->prev->next = el;

    return 0;
}

int ll_get_size(ll_t* l)
{
    return l->count;
}

int ll_copy(ll_t* dest, ll_t* src)
{
    if(dest == NULL)
        return 1;

    for(int i = 0; i < (int) src->count; ++i)
    {
        ll_element* el = (ll_element*) malloc(sizeof(ll_element));
        if(el== NULL)
            return 1;
        ll_insert(dest, ll_get_data_at(src, i));
    }

    return 0;
}

int ll_empty(ll_t* l)
{
    if(l == NULL)
        return 1;

    int c = l->count;
    for(int i = 0; i < c; ++i)
    {
        int err = ll_remove_at(l, 0);
        if(err)
            return 1;
    }

    return 0;
}
