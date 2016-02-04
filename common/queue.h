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
    U8      head;

    /// The tail index
    U8      tail;

    /// The element count, 0 => empty
    U8      count;

    /// The length of the queue
    U8      length;
} QueueU8;

/**
 * FIFO circular buffer of U16 values
 */
typedef struct
{
    /// The data pointer for the buffer
    U16*    data;

    /// The head index
    U8      head;

    /// The tail index
    U8      tail;

    /// The element count, 0 => empty
    U8      count;

    /// The length of the queue
    U8      length;
} QueueU16;

/**
 * Initializes a U8 queue
 *
 * @param   queue   The queue to initialize
 * @param   data    The backing data pointer of `length` bytes
 * @param   length  The length of the queue, must be a power of 2
 *
 * @return  0 on success, else an error code
 */
S32 queueU8Init(QueueU8* queue, U8* data, const U8 length);

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
 * Initializes a U16 queue
 *
 * @param   queue   The queue to initialize
 * @param   data    The backing data pointer of `length` bytes
 * @param   length  The length of the queue, must be a power of 2
 *
 * @return  0 on success, else an error code
 */
S32 queueU16Init(QueueU16* queue, U16* data, const U8 length);

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

#endif
