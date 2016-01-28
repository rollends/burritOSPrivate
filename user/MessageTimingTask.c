#include "kernel/sysCall.h"
#include "kernel/message.h"
#include "MessageTimingTask.h"

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