#include "kernel/assert.h"
#include "kernel/memoryAllocator.h"
#include "kernel/print.h"

S32 memoryAllocatorInit(MemoryAllocator* alloc, U32* base)
{
    alloc->baseAddress = base;
    assertOk(queueU16Init(&alloc->blockQueue,
                          alloc->queueData,
                          MEMORY_BLOCK_COUNT));

    U32 i;
    for (i = 1; i <= MEMORY_BLOCK_COUNT; i++)
    {
        assertOk(queueU16Push(&alloc->blockQueue, i));
    }

    return 0;
}

U32* memoryAllocatorAlloc(MemoryAllocator* alloc)
{
    U16 block;
    if (queueU16Pop(&alloc->blockQueue, &block) == 0)
    {
        U32* address = alloc->baseAddress +
                            ((U32)block * MEMORY_BLOCK_SIZE - 1);
        *address-- = block;
        return address;
    }

    return 0;
}

S32 memoryAllocatorFree(MemoryAllocator* alloc, U32* stack)
{
    U32 id = *(++stack);
    assertOk(queueU16Push(&alloc->blockQueue, id));
    return 0;
}
