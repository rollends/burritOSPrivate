#include <assert.h>
#include <stdio.h>

#include <byteBuffer.h>

unsigned char data[128];

void test_init(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
    assert(byteBufferInit(0, data, 128) == BUFFER_ERROR_NULLB);
    assert(byteBufferInit(buffer, 0, 128) == BUFFER_ERROR_NULLD);
#endif

#ifdef BOUNDS_CHECK
    assert(byteBufferInit(buffer, data, 0) == BUFFER_ERROR_LEN);
#endif

    assert(byteBufferInit(buffer, data, 128) == 0);
}

void test_peek(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
	assert(byteBufferPeek(0) == BUFFER_ERROR_NULLB);
#endif

#ifdef BOUNDS_CHECK	
	assert(byteBufferPeek(buffer) == BUFFER_ERROR_EMPTY);
#endif

	byteBufferPut(buffer, 'a');
	assert(byteBufferPeek(buffer) == 'a');
	assert(byteBufferCount(buffer) == 1);
	byteBufferGet(buffer);
}

void test_get(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
	assert(byteBufferGet(0) == BUFFER_ERROR_NULLB);
#endif

#ifdef BOUNDS_CHECK	
	assert(byteBufferGet(buffer) == BUFFER_ERROR_EMPTY);
#endif

	byteBufferPut(buffer, 'a');
	assert(byteBufferGet(buffer) == 'a');
	assert(byteBufferCount(buffer) == 0);
}

void test_put(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
	assert(byteBufferPut(0, 'a') == BUFFER_ERROR_NULLB);
#endif

	int i;
	for (i = 0; i < 128; i++)
	{
		assert(byteBufferPut(buffer, i) == 0);
	}

#ifdef BOUNDS_CHECK	
	assert(byteBufferPut(buffer, i) == BUFFER_ERROR_FULL);
#endif
}

void test_count(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
	assert(byteBufferCount(0) == BUFFER_ERROR_NULLB);
#endif

	assert(byteBufferCount(buffer) == 128);
	int i;
	for (i = 128; i >= 0; i--)
	{
		assert(byteBufferCount(buffer) == i);
		byteBufferGet(buffer);
	}
}

void test_empty(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
	assert(byteBufferEmpty(0) == BUFFER_ERROR_NULLB);
#endif

	assert(byteBufferEmpty(buffer) == 1);

	int i;
	for (i = 0; i < 128; i++)
	{
		byteBufferPut(buffer, 'a');
		assert(byteBufferEmpty(buffer) == 0);
	}
}

void test_full(ByteBuffer* buffer)
{
#ifdef NULL_CHECK
	assert(byteBufferFull(0) == BUFFER_ERROR_NULLB);
#endif

	assert(byteBufferFull(buffer) == 1);
	
	int i;
	for (i = 0; i < 128; i++)
	{
		byteBufferGet(buffer);
		assert(byteBufferFull(buffer) == 0);
	}
}

int main ()
{
    ByteBuffer buffer;
    test_init(&buffer);
    test_peek(&buffer);
    test_get(&buffer);
    test_put(&buffer);
    test_count(&buffer);
    test_empty(&buffer);
    test_full(&buffer);

    return 0;
}
