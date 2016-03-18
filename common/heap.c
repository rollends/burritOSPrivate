#include "common/heap.h"

// Macro Defines that find the index of various nodes in the heap relative to some
// start node index.
#define HEAP_RIGHT_CHILD( i ) ( ((i) + 1) << 1 )
#define HEAP_LEFT_CHILD( i ) ( HEAP_RIGHT_CHILD( i ) - 1 )
//#define HEAP_PARENT( i ) ( ((i) & 1) ? ((((i) + 1) >> 2) - 1) : (((i) >> 2) - 1) )
#define HEAP_PARENT( i ) ( (((i) + ((i) & 1)) >> 1) - 1 )
//#define HEAP_SIBLING( i ) ( ((i) & 1) ? ((i) + 1) : ((i) - 1) )
#define HEAP_SIBLING( i ) ( (i) - 1 + ( ((i) & 1) << 1 ) )

S32 heapU16Init(HeapU16* heap, HeapNodeU16* array, U32 count)
{
    U32 i = 0;
    for(i = 0; i < (count-1); ++i)
    {
        array[i].next = &array[i+1];
        array[i].isFilled = 0;
    }
    array[i].next = 0;
    array[i].isFilled = 0;

    heap->freeList = array;
    heap->heap = array;
    heap->size = count;
    return 0;
}

S32 heapU16Push(HeapU16* heap, HeapNodeU16 node)
{
    if( heap->freeList == 0 ) return -1;

    U16 index = heap->freeList - heap->heap;
    heap->freeList = heap->freeList->next;

    HeapNodeU16* current = heap->heap + index;
    HeapNodeU16* parent = heap->heap + HEAP_PARENT(index);
    
    node.isFilled = 1;
    
    while(index != 0 && node.cost <= parent->cost)
    {
        // Swap parent into our location
        *current = *parent;
        
        // Go up the tree
        index = HEAP_PARENT(index);
        current = heap->heap + index;
        parent = heap->heap + HEAP_PARENT(index);
    }

    // Finally do the insert (dirty tricks 4 lyfe)
    *current = node;
    return 0;
}

S32 heapU16Pop(HeapU16* heap, HeapNodeU16* node)
{
    if(!heap->heap->isFilled) return -1;
 
    *node = *heap->heap;

    HeapNodeU16* current = heap->heap;

    do
    {
        HeapNodeU16* left = heap->heap + HEAP_LEFT_CHILD( current - heap->heap );
        HeapNodeU16* right = heap->heap + HEAP_RIGHT_CHILD( current - heap->heap );

        if(left->isFilled && right->isFilled)
        {
            if(right->cost <= left->cost)
            {
                *current = *right;
                current = right;
            }
            else
            {
                *current = *left;
                current = left;
            }
        }
        else if(left->isFilled)
        {
            *current = *left;
            current = left;
        }
        else if(right->isFilled)
        {
            *current = *right;
            current = right;
        }
        else
        {
            break;
        }
    } while( HEAP_LEFT_CHILD(current - heap->heap) < heap->size );
    
    current->isFilled = 0;
    current->next = heap->freeList;
    heap->freeList = current;
    return 0;
}
