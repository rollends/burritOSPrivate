#include "hardware/memory.h"

#include "kernel/systemCall.h"
#include "kernel/message.h"

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
    sysExit();
}

void MessageTimingTask64()
{
    MessageEnvelope envelope;
    U32 buffer[16];
    
    U16 id;
    for(;;)
    {
        sysReceive( &id, &envelope );
        __memcpy(buffer, envelope.message.MessageArbitrary.body, 16);
        envelope.message.MessageArbitrary.body = buffer;
        sysReply( id, &envelope );
        if( envelope.type == 0 )
            break;
    }
    sysExit();
}
