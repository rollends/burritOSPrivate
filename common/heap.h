#ifndef HEAP_H
#define HEAP_H

#include "common/types.h"

typedef struct HeapNodeU16
{
    U16                     value;
    U16                     cost;
    U8                      isFilled;
    struct HeapNodeU16*     next;
} HeapNodeU16;

typedef struct
{
    U32                     size;
    HeapNodeU16*            heap;
    HeapNodeU16*            freeList;
} HeapU16;

S32 heapU16Init(HeapU16* heap, HeapNodeU16* array, U32 size);
S32 heapU16Push(HeapU16*, HeapNodeU16);
S32 heapU16Pop(HeapU16*, HeapNodeU16*);

#define HEAP_NODE( VALUE, COST ) { VALUE, COST, 0, 0 }

#endif
