//This file is used to implement the board-ht

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "lib/bitvector.h"
#include "lib/hdict.h"
#include "board-ht.h"

bool key_equal_client(hdict_key x, hdict_key y)
{
    REQUIRES(x!=NULL && y!=NULL);
    bitvector x1 = *(bitvector*)x;
    bitvector y1 = *(bitvector*)y;
    return bitvector_equal(x1, y1);
}

size_t key_hash_client(hdict_key x)
{
    REQUIRES(x!=NULL);
    size_t res =0;
    bitvector x1 = *(bitvector*)x;
    for(int i =0;i<BITVECTOR_LIMIT;i++)
    {
        if(bitvector_get(x1,i))
            {
                res = res*149+3449;
                res = res+i*i;
            }
    }
    return res;
}

void value_free_client(hdict_value x)
{
    REQUIRES(x!=NULL);
    board_data* BA = (board_data*)x;
    free(BA);
}

hdict_t ht_new(size_t capacity)
{
    REQUIRES(capacity>0);
    hdict_t H = hdict_new(capacity, &key_equal_client,&key_hash_client,
                          &value_free_client);
    ENSURES(H!=NULL);
    return H;
}

void ht_insert(hdict_t H, struct board_data *DAT)
{
    REQUIRES(H!=NULL);
    //if(DAT==NULL)
    //    return;
    //REQUIRES(ht_lookup(H,DAT->board)==NULL);
    bitvector* B = &(DAT->board);
    //hdict_value res =
    hdict_insert(H, (void*)B, (void*)DAT);


    //if(res!=NULL)
    //{
    //    value_free_client(res);
    //}
}

struct board_data *ht_lookup(hdict_t H, bitvector B)
{
    REQUIRES(H!=NULL);
    bitvector* p = &B;
    return (board_data*)hdict_lookup(H,(void*)p);
}

