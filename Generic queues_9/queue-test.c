//This file is used to test the C version of queue
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

void free_node(void *x)
{
    free(x);
}

int main()
{
    queue_t Q = queue_new();
    //ASSERT(Q->size==0);
    int* i1 = xmalloc(sizeof(int));
    *i1 = 1;
    enq(Q,(void*)i1);
    int* i2 = xmalloc(sizeof(int));
    *i2 = 2;
    enq(Q,(void*)i2);

    //ASSERT(Q->size==2);
    ASSERT(*(int*)(queue_peek(Q,0))==1);
    ASSERT(*(int*)(queue_peek(Q,1))==2);

    queue_reverse(Q);
    ASSERT(*(int*)(queue_peek(Q,0))==2);
    ASSERT(*(int*)(queue_peek(Q,1))==1);
    void* res1 = deq(Q);
    void* res2 = deq(Q);
    ASSERT(*(int*)res1==2);
    ASSERT(*(int*)res2==1);
    //ASSERT(Q->size==0);
    free(i1);
    free(i2);
    queue_free(Q,&free_node);

    printf("All tests passed!\n");

    return 0;


}
