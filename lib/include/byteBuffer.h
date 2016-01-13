#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#define BUFFER_ERROR_NULLB  -1
#define BUFFER_ERROR_NULLD  -2
#define BUFFER_ERROR_LEN    -3
#define BUFFER_ERROR_EMPTY  -4
#define BUFFER_ERROR_FULL   -5

/**
 * Defines a circular buffer of unsigned characters
 */
typedef struct
{
    /// Points to buffer memory
    unsigned char* data;

    /// The head index in memory
    unsigned short head;

    /// The tail index in memory
    unsigned short tail;

#ifdef BOUNDS_CHECK
    /// The bounds checking counter
    unsigned short count;
#endif

    /// The length of the buffer
    unsigned short length;

    /// Flag indicating if the buffer is full
    unsigned char full;
} ByteBuffer;

/**
 * Initializes a buffer with a data pointer and length
 *
 * @param   buffer  The buffer to initialize
 * @param   data    A pointer to a block of `length` bytes of memory
 * @param   length  The length of the buffer, must be greater than 1 and less than 2^16
 * @return  0 if the buffer is successfully initialized, else an error code
 */
int byteBufferInit(ByteBuffer* buffer, unsigned char* data, const unsigned short length);

/**
 * Peeks at first byte of a buffer without removing it.
 *
 * @param   buffer  The buffer to read from
 * @return  0..255 if a byte is succesfully read, else an error code
 */
int byteBufferPeek(ByteBuffer* buffer);

/**
 * Removes the first byte of a buffer and returns it
 *
 * @param   buffer  The buffer to read from
 * @return  0..255 if a byte is succesfully removed, else an error code
 */
int byteBufferGet(ByteBuffer* buffer);

/**
 * Appends a byte to the end of a buffer
 *
 * @param   buffer  The buffer to append to
 * @return  0 if the byte is append, else an error code
 */
int byteBufferPut(ByteBuffer* buffer, const unsigned char byte);

/**
 * Checks the number of bytes stored in the buffer
 *
 * @param   buffer  The buffer to inspect
 * @return  the number of the bytes or an error code
 */
int byteBufferCount(ByteBuffer* buffer);

/**
 * Checks if the buffer is empty
 *
 * @param   buffer  The buffer to inspect
 * @return  1 if the buffer is empty, 0 if it is not, else an error code
 */
int byteBufferEmpty(ByteBuffer* buffer);

/**
 * Checks if the buffer is full
 *
 * @param   buffer  The buffer to inspect
 * @return  1 if the buffer is full, 0 if it is not, else an error code
 */
int byteBufferFull(ByteBuffer* buffer);

#endif