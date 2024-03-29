#ifndef QUEUE_H
#define QUEUE_H

#include "common/types.h"

#define QUEUE_MEM_SIZE(length, type) sizeof(type)*length

/**
 * FIFO circular buffer of U8 values
 */
typedef struct
{
    /// The data pointer for the buffer
    U8*     data;

    /// The head index
    U32     head;

    /// The tail index
    U32     tail;

    /// The element count, 0 => empty
    U32     count;

    /// The length of the queue
    U32     length;
} QueueU8;

/**
 * FIFO circular buffer of U16 values
 */
typedef struct
{
    /// The data pointer for the buffer
    U16*    data;

    /// The head index
    U32     head;

    /// The tail index
    U32     tail;

    /// The element count, 0 => empty
    U32     count;

    /// The length of the queue
    U32     length;
} QueueU16;

/**
 * FIFO circular buffer of U32* values
 */
typedef struct
{
    /// The data pointer for the buffer
    U32**   data;

    /// The head index
    U32     head;

    /// The tail index
    U32     tail;

    /// The element count, 0 => empty
    U32     count;

    /// The length of the queue
    U32     length;
} QueuePU32;

/**
 * Initializes a U8 queue
 *
 * @param   queue   The queue to initialize
 * @param   data    The backing data pointer of `length` bytes
 * @param   length  The length of the queue, must be a power of 2
 *
 * @return  0 on success, else an error code
 */
S32 queueU8Init(QueueU8* queue, U8* data, const U32 length);

/**
 * Enqueues a value in a U8 queue
 *
 * @param   queue   The queue to initialize
 * @param   value   The value to enqueue
 *
 * @return  0 on success, else an error code
 */
S32 queueU8Push(QueueU8* queue, const U8 value);

/**
 * Dequeues a value from a U8 queue
 *
 * @param   queue   The queue to initialize
 * @param   value   The destination to load the value
 *
 * @return  0 on success, else an error code
 */
S32 queueU8Pop(QueueU8* queue, U8* value);

/**
 * Peeks at the value at the head of the U8 Queue.
 * Does not change the queue. Will not change the 
 * return value (2nd parameter) if there is nothing
 * at the head.
 *
 * @param   queue   The queue to peek in.
 * @param   value   The destination to load the value 
 *                  *if* there is a value to load.
 * 
 * @return  0 on success, else an error code (if nothing to load)
 */
S32 queueU8Peek(QueueU8* queue, U8* value);

/**
 * Initializes a U16 queue
 *
 * @param   queue   The queue to initialize
 * @param   data    The backing data pointer of `length` bytes
 * @param   length  The length of the queue, must be a power of 2
 *
 * @return  0 on success, else an error code
 */
S32 queueU16Init(QueueU16* queue, U16* data, const U32 length);

/**
 * Enqueues a value in a U16 queue
 *
 * @param   queue   The queue to initialize
 * @param   value   The value to enqueue
 *
 * @return  0 on success, else an error code
 */
S32 queueU16Push(QueueU16* queue, const U16 value);

/**
 * Dequeues a value from a U16 queue
 *
 * @param   queue   The queue to initialize
 * @param   value   The destination to load the value
 *
 * @return  0 on success, else an error code
 */
S32 queueU16Pop(QueueU16* queue, U16* value);


/**
 * Peeks at the value at the head of the U16 Queue.
 * Does not change the queue. Will not change the 
 * return value (2nd parameter) if there is nothing
 * at the head.
 *
 * @param   queue   The queue to peek in.
 * @param   value   The destination to load the value 
 *                  *if* there is a value to load.
 * 
 * @return  0 on success, else an error code (if nothing to load)
 */
S32 queueU16Peek(QueueU16* queue, U16* value);

/**
 * Initializes a U32* queue
 *
 * @param   queue   The queue to initialize
 * @param   data    The backing data pointer of `length` bytes
 * @param   length  The length of the queue, must be a power of 2
 *
 * @return  0 on success, else an error code
 */
S32 queuePU32Init(QueuePU32* queue, U32** data, const U32 length);

/**
 * Enqueues a value in a U32* queue
 *
 * @param   queue   The queue to initialize
 * @param   value   The value to enqueue
 *
 * @return  0 on success, else an error code
 */
S32 queuePU32Push(QueuePU32* queue, U32 * const pointer);

/**
 * Dequeues a value from a U32* queue
 *
 * @param   queue   The queue to initialize
 * @param   value   The destination to load the value
 *
 * @return  0 on success, else an error code
 */
S32 queuePU32Pop(QueuePU32* queue, U32** value);


#endif
