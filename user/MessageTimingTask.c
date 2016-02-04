#include "kernel/kernel.h"

#include "user/messageTypes.h"
#include "user/MessageTimingTask.h"

void MessageTimingTask()
{
    MessageEnvelope envelope;

    U16 id;
    for(;;)
    {
        sysReceive( &id, &envelope );
        sysReply( id, &envelope );
        if( envelope.type == 0 )
            break;
    }
}

void MessageTimingTask64()
{
    MessageEnvelope envelope;
    U32 buffer[16];
    
    U16 id;
    for(;;)
    {
        sysReceive( &id, &envelope );
        memcpy(buffer, envelope.message.MessageArbitrary.body, 16);
        envelope.message.MessageArbitrary.body = buffer;
        sysReply( id, &envelope );
        if( envelope.type == 0 )
            break;
    }
}
