#include "kernel/stackAllocator.h"
#include "kernel/print.h"

static inline U32* blockToAddress(U16 block, U32* baseAddress)
{
    return baseAddress + ((U32)(block) * STACK_BLOCK_SIZE);
}

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
    for (i = 1; i <= STACK_SMALL_COUNT; i++)
    {
        queueU16Push(&alloc->smallQueue, i);
    }

    for (i = 1; i <= STACK_MEDIUM_COUNT; i++)
    {
        queueU16Push(&alloc->mediumQueue,
                     i*STACK_SIZE_MEDIUM + STACK_SMALL_COUNT);
    }

    for (i = 1; i <= STACK_MEDIUM_COUNT; i++)
    {
        queueU16Push(&alloc->mediumQueue,
                     i*STACK_SIZE_LARGE + STACK_MEDIUM_COUNT*STACK_SIZE_MEDIUM + STACK_SMALL_COUNT);
    }

    return 0;
}

U32 stackAllocatorAlloc(StackAllocator* alloc, const U32 size)
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
        return blockToAddress(block, alloc->baseAddress);
    }

    return 0;
}