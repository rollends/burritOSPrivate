#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "common/common.h"
#include "kernel/config.h"

/// The number of small blocks in the allocator
#define STACK_SMALL_COUNT   TASK_COUNT

/// The number of medium blocks in the allocator
#define STACK_MEDIUM_COUNT  8

/// The number of large blocks in the allocator
#define STACK_LARGE_COUNT   4

/**
 * Stack allocator state struct with memory block queues
 */
typedef struct
{
    /// Base stack address
    U32*        baseAddress;

    /// FIFO queue of small stack blocks
    QueueU16    smallQueue;

    /// FIFO queue of medium stack blocks
    QueueU16    mediumQueue;

    /// FIFO queue of large stackblocks
    QueueU16    largeQueue;

    /// Backing data for the queues
    U16         queueData[STACK_SMALL_COUNT +
                          STACK_MEDIUM_COUNT +
                          STACK_LARGE_COUNT];
} StackAllocator;

/**
 * Initializes a stack allocator instance at a given base address
 *
 * @param   alloc   The stack allocator pointer
 * @param   base    Base address for the allocator
 *
 * @return  0 on success, else an error code
 */
S32 stackAllocatorInit(StackAllocator* alloc, U32* base);

/**
 * Allocates a stack of a given size
 *
 * @param   alloc   The stack allocator pointer
 * @param   size    The stack size
 *
 * @return  0 (null pointer) on failure, else a memory address
 */
U32 stackAllocatorAlloc(StackAllocator* alloc, const U32 size);

#endif
