//This file is the implementation of bitvector.h file
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/bitvector.h"

bitvector bitvector_new()
{
    return (bitvector)0;
}

bool bitvector_get(bitvector B, uint8_t i)
{
    REQUIRES(i<BITVECTOR_LIMIT);
    return(((B>>i)&1)==1);
}

bitvector bitvector_flip(bitvector B, uint8_t i)
{
    REQUIRES(i<BITVECTOR_LIMIT);
    bitvector mask = 1<<i;
    return B ^ mask;
}

bool bitvector_equal(bitvector B1, bitvector B2)
{
    return B1==B2;
}
