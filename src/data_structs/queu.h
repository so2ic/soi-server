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
    queue_element_t *head, *tail;
    size_t size;
    QUEUE_MUTEX_TYPE mutex;
} queue_t;

QUEUE_LIB queue_t* queue_init();
QUEUE_LIB int queue_add(queue_t* q, void* obj);
QUEUE_LIB int queue_free(queue_t* q);
QUEUE_LIB void* queue_dequeue(queue_t* q);

#ifdef QUEUE_LIB_IMPLEMENTATION

QUEUE_LIB queue_t* queue_init()
{
    queue_t* q = (queue_t*) QUEUE_MALLOC(sizeof(queue_t));
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

    queue_elemetn* el;

    if((el = (queue_element*) QUEUE_MALLOC(sizeof(queue_element))) == NULL)
    {
        QUEUE_MUTEX_UNLOCK(q->mutex);
        return 0;
    }

    el->object = obj;

    if(q.size == 0)
    {
        el->prev = NULL;
        q->head = el;
    }
    else
    {
        q->tail->prev = el;
    }

    q->tail = el;
    q.size++;

    QUEUE_MUTEX_UNLOCK(q->mutex);
    return 1;
}

QUEUE_LIB int queue_free(queue_t* q)
{
    if(q == NULL)
        return 0;

    queu_element* l = q->head;
    for(int i = 0; i < q->size; ++i)
    {
        queu_element* n = l->prev;
        QUEUE_FREE(l);
        l = n;
    }

    q->size = 0;
    q->head = NULL;
    q->tail = NULL;
    QUEUE_MUTEX_DESTROY(q->mutex);
    free(q);
    return 1;
}

#endif // QUEUE_LIB_IMPLEMENTATION

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QUEUE_H
