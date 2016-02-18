#include "kernel/kernel.h"
#include "user/messageTypes.h"
#include "user/services/services.h"

void clockDelayBy( TaskID clockId, U32 ticks )
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_CLOCKSERVER_DELAY_BY;
    envelope.message.MessageU32.body = ticks;
    sysSend( clockId.value, &envelope, &envelope );
}

void clockLongDelayBy( TaskID clockId, U32 mult150ms )
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_CLOCKSERVER_LONG_DELAY_BY;
    envelope.message.MessageU32.body = mult150ms;
    sysSend( clockId.value, &envelope, &envelope );
}

U32 clockTime( TaskID clock )
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_CLOCKSERVER_GET_TIME;
    sysSend( clock.value, &envelope, &envelope );
    return envelope.message.MessageU32.body;
}

void clockDelayUntil( TaskID clock, U32 v )
{
    U32 t = clockTime( clock );
    if( v > t )
        clockDelayBy( clock, v - t );
    else if( v < t )
        clockDelayBy( clock, v + (0xFFFFFFFF - t) );
}


