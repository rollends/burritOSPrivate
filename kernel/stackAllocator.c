#include "kernel/stackAllocator.h"
#include "kernel/print.h"

S32 stackAllocatorInit(StackAllocator* alloc, U32* base)
{
    alloc->baseAddress = base;
    queueU16Init(&alloc->smallQueue, alloc->queueData, STACK_SMALL_COUNT);
    queueU16Init(&alloc->mediumQueue,
                 alloc->queueData + STACK_SMALL_COUNT,
                 STACK_MEDIUM_COUNT);
    queueU16Init(&alloc->largeQueue,
                 alloc->queueData + STACK_SMALL_COUNT + STACK_MEDIUM_COUNT,
                 STACK_LARGE_COUNT);

    U32 i;
    for (i = 0; i < STACK_SMALL_COUNT; i++)
    {
        queueU16Push(&alloc->smallQueue, i);
    }

    for (i = 0; i < STACK_MEDIUM_COUNT; i++)
    {
        queueU16Push(&alloc->mediumQueue,
                     i*STACK_SIZE_MEDIUM + STACK_SMALL_COUNT);
    }

    for (i = 0; i < STACK_SIZE_LARGE; i++)
    {
        queueU16Push(&alloc->largeQueue,
                     i*STACK_SIZE_LARGE + STACK_MEDIUM_COUNT*STACK_SIZE_MEDIUM + STACK_SMALL_COUNT);
    }

    return 0;
}

U32* stackAllocatorAlloc(StackAllocator* alloc, const U32 size)
{
    U16 block;
    QueueU16* queue = 0;

    if (size == STACK_SIZE_SMALL)
    {
        queue = &alloc->smallQueue;
    }
    else if (size == STACK_SIZE_MEDIUM)
    {
        queue = &alloc->mediumQueue;
    }
    else if (size == STACK_SIZE_LARGE)
    {
        queue = &alloc->largeQueue;
    }
    else
    {
        return 0;
    }

    if (queueU16Pop(queue, &block) == 0)
    {
        U32* address = alloc->baseAddress +
                            ((U32)(block+1) * STACK_BLOCK_SIZE - 1);
        *address-- = block;
        *address-- = size;
        return address;
    }

    return 0;
}

S32 stackAllocatorFree(StackAllocator* alloc, U32* stack)
{
    U32 size = *(++stack);
    U32 id = *(++stack);
    QueueU16* queue = 0;

    if (size == STACK_SIZE_SMALL)
    {
        queue = &alloc->smallQueue;
    }
    else if (size == STACK_SIZE_MEDIUM)
    {
        queue = &alloc->mediumQueue;
    }
    else if (size == STACK_SIZE_LARGE)
    {
        queue = &alloc->largeQueue;
    }
    else
    {
        return -1;
    }

    queueU16Push(queue, id);
    return 0;
}
