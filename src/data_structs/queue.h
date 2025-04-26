/*
------------------------------------------------------------------------------
QUEUE LIB FOR C - ASTOLFI Vincent 2025

Usage:

    #define QUEUE_LIB_IMPLEMENTATION
    // if no thread concurrencie in your program
    #define QUEUE_NO_THREADING
    #include "queue.h"

API:

    queue_t* queue_init();
    int queue_add(queue_t* q, void* obj);
    int queue_add_many(queue_t* q, ...);
    int queue_free(queue_t* q);
    void* queue_dequeue(queue_t* q);

RETURNS:

    - queue_init()
    return the new created queue, NULL on error

    - queue_add()
    return 1 on success, 0 on error

    - queue_free()
    return 1 on success, 0 on error

    - queue_dequeue()
    return front object of the queue, NULL on error

EXAMPLE:

    ```c
    #define QUEUE_LIB_IMPLEMENTATION
    #define QUEUE_NO_THREADING
    #include "queue.h"

    #include <stdlib.h>
    #include <stdio.h>

    int main(void)
    {
        char* foo = "foo";
        char* bar = "bar";
        char* baz = "baz";
        char* qux = "qux";

        queue_t* queue;

        if((queue = queue_init()) == NULL)
            return 1;

        if(!queue_add(queue, foo))
            return 1;

        if(!queue_add_many(queue, bar, baz, qux))
            return 1;

        char* c;
        if((c = queue_dequeue(queue)) == NULL)
            return 1;

        printf("EXPECT : foo , GET : %s \n", c);

        if(!queue_free(queue))
            return 1;
            
        return 0;
    }
    ```
------------------------------------------------------------------------------
*/

#ifndef QUEUE_H
#define QUEUE_H

#if defined(QUEUE_NO_THREADING)
    #define     QUEUE_MUTEX_TYPE        int
    #define     QUEUE_MUTEX_INIT(x)     ((void)0)
    #define     QUEUE_MUTEX_LOCK(x)     ((void)0)
    #define     QUEUE_MUTEX_UNLOCK(x)   ((void)0)
    #define     QUEUE_MUTEX_DESTROY(x)  ((void)0)
#elif !defined(QUEUE_MUTEX_TYPE)
    #include    <pthread.h>
    #define     QUEUE_MUTEX_TYPE        pthread_mutex_t
    #define     QUEUE_MUTEX_INIT(m)     pthread_mutex_init(&(m), NULL)
    #define     QUEUE_MUTEX_LOCK(m)     pthread_mutex_lock(&(m))
    #define     QUEUE_MUTEX_UNLOCK(m)   pthread_mutex_unlock(&(m))
    #define     QUEUE_MUTEX_DESTROY(m)  pthread_mutex_destroy(&(m))
#endif

#ifndef QUEUE_MALLOC
    #include <stdlib.h>
    #define QUEUE_MALLOC malloc
#endif // QUEUE_MALLOC

#ifndef QUEUE_FREE
    #include <stdlib.h>
    #define QUEUE_FREE free
#endif // QUEUE_FREE

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef QUEUE_LIB
    #define QUEUE_LIB static inline
#endif // QUEUE_LIB


typedef struct queue_element queue_element;

struct queue_element
{
    void* object;
    queue_element *prev;
};

typedef struct
{
    queue_element *head, *tail;
    size_t size;
    QUEUE_MUTEX_TYPE mutex;
} queue_t;

QUEUE_LIB queue_t* queue_init();
QUEUE_LIB int queue_add(queue_t* q, void* obj);
QUEUE_LIB int queue_free(queue_t* q);
QUEUE_LIB void* queue_dequeue(queue_t* q);

#ifdef QUEUE_LIB_IMPLEMENTATION

#define queue_add_many(q, ...) queue_add_many_null(q, __VA_ARGS__, NULL)

QUEUE_LIB queue_t* queue_init()
{
    queue_t* q = (queue_t*) QUEUE_MALLOC(sizeof(queue_t));
    if(q == NULL)
        return NULL;
    
    q->size = 0;
    q->head = NULL;
    q->tail = NULL;
    QUEUE_MUTEX_INIT(q->mutex);

    return q;
}

QUEUE_LIB int queue_add(queue_t* q, void* obj)
{
    if(q == NULL || obj == NULL)
       return 0; 

    QUEUE_MUTEX_LOCK(q->mutex);

    queue_element* el;

    if((el = (queue_element*) QUEUE_MALLOC(sizeof(queue_element))) == NULL)
    {
        QUEUE_MUTEX_UNLOCK(q->mutex);
        return 0;
    }

    el->object = obj;

    if(q->size == 0)
    {
        el->prev = NULL;
        q->head = el;
    }
    else
    {
        q->tail->prev = el;
    }

    q->tail = el;
    q->size++;

    QUEUE_MUTEX_UNLOCK(q->mutex);
    return 1;
}

static int queue_add_many_null(queue_t* q, ...)
{
    if (!q) return 0;

    va_list args;
    va_start(args, q);

    void* obj;
    while ((obj = va_arg(args, void*)) != NULL) {
        if (!queue_add(q, obj)) {
            va_end(args);
            return 0;
        }
    }

    va_end(args);
    return 1;
}

QUEUE_LIB int queue_free(queue_t* q)
{
    if(q == NULL)
        return 0;

    queue_element* l = q->head;
    while(l != NULL)
    {
        queue_element* n = l->prev;
        QUEUE_FREE(l);
        l = n;
    }

    q->size = 0;
    q->head = NULL;
    q->tail = NULL;
    QUEUE_MUTEX_DESTROY(q->mutex);
    QUEUE_FREE(q);
    return 1;
}

QUEUE_LIB void* queue_dequeue(queue_t* q)
{
    if(q == NULL)
        return NULL;

    QUEUE_MUTEX_LOCK(q->mutex);

    queue_element* el = q->head;
    if(!el)
    {
        QUEUE_MUTEX_UNLOCK(q->mutex);
        return NULL;
    }

    void* obj = el->object;
    q->head = el->prev;

    if(q->head == NULL)
        q->tail = NULL;

    QUEUE_FREE(el);
    QUEUE_MUTEX_UNLOCK(q->mutex);
    q->size--;

    return obj;
}

#endif // QUEUE_LIB_IMPLEMENTATION

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QUEUE_H

