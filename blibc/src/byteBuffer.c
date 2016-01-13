#include "byteBuffer.h"

int byteBufferInit(ByteBuffer* buffer, unsigned char* data, const unsigned short length)
{
#ifdef NULL_CHECK
    if (buffer == 0)
    {
        return BUFFER_ERROR_NULLB;
    }

    if (data == 0)
    {
        return BUFFER_ERROR_NULLD;
    }
#endif

#ifdef BOUNDS_CHECK
    if (length < 1)
    {
        return BUFFER_ERROR_LEN;
    }
#endif

    buffer->data = data;
    buffer->head = 0;
    buffer->tail = 0;
    buffer->length = length;
    buffer->full = 0;

    return 0;
}

int byteBufferPeek(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
    if (buffer == 0)
    {
        return BUFFER_ERROR_NULLB;
    }
#endif

#ifdef BOUNDS_CHECK
    if (byteBufferCount(buffer) == 0)
    {
        return BUFFER_ERROR_EMPTY;
    }
#endif

    return buffer->data[buffer->head];
}

int byteBufferGet(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
    if (buffer == 0)
    {
        return BUFFER_ERROR_NULLB;
    }
#endif

#ifdef BOUNDS_CHECK
    if (byteBufferCount(buffer) == 0)
    {
        return BUFFER_ERROR_EMPTY;
    }
#endif

    int b = buffer->data[buffer->head];
#if ZERO_MEMORY
    buffer->data[buffer->head] = 0;
#endif
    buffer->head = (buffer->head + 1) % buffer->length;
    buffer->full = 0;

    return b;
}

int byteBufferPut(ByteBuffer* buffer, const unsigned char byte)
{
#ifdef NULL_CHECK
    if (buffer == 0)
    {
        return BUFFER_ERROR_NULLB;
    }
#endif

#ifdef BOUNDS_CHECK
    if (byteBufferCount(buffer) == buffer->length)
    {
        return BUFFER_ERROR_FULL;
    }
#endif

    buffer->data[buffer->tail] = byte;
    buffer->tail = (buffer->tail + 1) % buffer->length;
    buffer->full = (buffer->tail == buffer->head);

    return 0;
}

int byteBufferCount(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
    if (buffer == 0)
    {
        return BUFFER_ERROR_NULLB;
    }
#endif

    if (buffer->tail >= buffer->head)
    {
        if (buffer->full == 0)
        {
            return buffer->tail - buffer->head;
        }
        else
        {
            return buffer->length;
        }
    }
    else
    {
        return buffer->length - buffer->head + buffer->tail;
    }
}

int byteBufferEmpty(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
    if (buffer == 0)
    {
        return BUFFER_ERROR_NULLB;
    }
#endif

    if (buffer->full == 0 &&
        (buffer->head == buffer->tail))
    {
        return 1;
    }

    return 0;
}

int byteBufferFull(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
    if (buffer == 0)
    {
        return BUFFER_ERROR_NULLB;
    }
#endif

    if (buffer->full != 0 &&
        (buffer->head == buffer->tail))
    {
        return 1;
    }

    return 0;
}
