#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "common/common.h"
#include "kernel/config.h"

/**
 * Stack allocator state struct with memory block queues
 */
typedef struct
{
    /// Base stack address
    U32*        baseAddress;

    /// FIFO queue of small stack blocks
    QueueU16    blockQueue;

    /// Backing data for the queues
    U16         queueData[MEMORY_BLOCK_COUNT];
} MemoryAllocator;

/**
 * Initializes a stack allocator instance at a given base address
 *
 * @param   alloc   The stack allocator pointer
 * @param   base    Base address for the allocator
 *
 * @return  0 on success, else an error code
 */
S32 memoryAllocatorInit(MemoryAllocator* alloc, U32* base);

/**
 * Allocates a stack of a given size
 *
 * @param   alloc   The stack allocator pointer
 * @param   size    The stack size
 *
 * @return  0 (null pointer) on failure, else a memory address
 */
U32* memoryAllocatorAlloc(MemoryAllocator* alloc);

/**
 * Frees a stack block
 *
 * @param   alloc   The stack allocator pointer
 * @param   size    The stack size
 *
 * @return  0 on success, else an error code
 */
S32 memoryAllocatorFree(MemoryAllocator* alloc, U32* stack);

#endif
