//This file is the c version of queue implementation
#include <stdlib.h>
#include <stdbool.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

typedef struct list_node list;
struct list_node {
  void* data;
  list* next;
};

typedef struct queue_header queue;
struct queue_header {
  list* front;
  list* back;
  size_t size;
};

bool is_inclusive_segment(list* start, list* end, size_t i) {
  /* FIX ME */
  if (start==NULL)
    return i==0;
  else if(start==end)
    return start->next==NULL&&i==1;
  else if(start!=end)
    return i>1&&is_inclusive_segment(start->next,end,i-1);
  else
    return false;
}

bool is_queue(queue* Q)
{
  return Q!=NULL&&is_inclusive_segment(Q->front, Q->back, Q->size);
}

queue* queue_new()
{
    queue* Q = xmalloc(sizeof(struct queue_header));
    Q->front = NULL;
    Q->size =0;
    ENSURES(Q!=NULL);
    ENSURES(is_queue(Q));
    return Q;
}

size_t queue_size(queue* Q)
{
    REQUIRES(Q!=NULL);
    REQUIRES(is_queue(Q));
    //ENSURES(Q->size>=0);
    return Q->size;
}

void enq(queue* Q, void* x)
{
    REQUIRES(Q!=NULL);
    REQUIRES(is_queue(Q));
    list* node = xmalloc(sizeof(struct list_node));
    node->data = x;
    node->next = NULL;
    //if the Q is empty
    if(Q->size==0)
    {
        Q->front = node;
        Q->back = node;
        Q->size = Q->size+1;
    }
    //if the Q is not empty
    else
    {
        Q->back->next = node;
        Q->back = node;
        Q->size=Q->size+1;
    }
}

void* deq(queue* Q)
{
    REQUIRES(Q!=NULL && queue_size(Q)>0);
    void* res = Q->front->data;
    list* del = Q->front;
    Q->front = Q->front->next;
    free(del);
    Q->size = Q->size-1;
    return res;
}

void* queue_peek(queue* Q, size_t i)
{
    REQUIRES(Q!=NULL && is_queue(Q) &&i<queue_size(Q));
    list* tmp = Q->front;
    for(size_t k=0;k<i;k++)
    {
        Q->front = Q->front->next;
    }
    if(Q->front==NULL)
        return NULL;
    void* res = Q->front->data;
    Q->front = tmp;
    return res;
}

void queue_reverse(queue* Q)
{
    REQUIRES(Q!=NULL && is_queue(Q));
    if(Q->size==0||Q->size==1)
        return;
    list* prev_front = Q->front;
    //list* prev_back = Q->back;

    list* p1 = Q->front;
    list* p2 = Q->front->next;

    while(p1!=Q->back)
    {
        list* tmp = p2->next;
        p2->next = p1;
        p1 = p2;
        p2 = tmp;
    }
    ASSERT(p1==Q->back);
    Q->front = p1;
    Q->back = prev_front;
    Q->back->next = NULL;

}

bool queue_all(queue* Q, check_property_fn* P)
{
    REQUIRES(Q!=NULL && P!=NULL);
    REQUIRES(is_queue(Q));
    if(Q->front == NULL)
        return true;
    list* tmp = Q->front;
    while(tmp!= NULL)
    {
        if((*P)(tmp->data)==false)
            return false;
        tmp = tmp->next;
    }
    return true;
}

void* queue_iterate(queue* Q, void* base, iterate_fn* F)
{
    REQUIRES(Q!=NULL && F!=NULL);
    REQUIRES(is_queue(Q));
    if(Q->front==NULL)
        return base;
    list* tmp = Q->front;
    while(tmp!=NULL)
    {
        base = (*F)(base,tmp->data);
        tmp = tmp->next;
    }
    return base;
}

void queue_free(queue* Q, elem_free_fn* F)
{
    REQUIRES(Q!=NULL);
    //REQUIRES(is_queue(Q));
    if(F==NULL)
        free(Q);
    else
    {
        if(Q->front==NULL)
            free(Q);
        else // the queue is not empty
        {
            list* p = Q->front;
            while(Q->front!=NULL)
            {
                p = Q->front->next;
                (*F)((void*)Q->front);
                Q->front = p;
            }
            free(Q);
        }
    }
}
