#include <assert.h>
#include <byteBuffer.h>

unsigned char data[128];

void test_init(ByteBuffer* buffer)
{
    assert(byteBufferInit(buffer, data, 128) == 0);
}

int main ()
{
    ByteBuffer buffer;

    return 0;
}
