#include "kernel/kernel.h"
#include "user/messageTypes.h"

void ClockNotifier()
{
    MessageEnvelope envelope;
    TaskID id = { sysPid() };

    envelope.type = MESSAGE_CLOCKSERVER_NOTIFY_TICK;
    for(;;)
    {
        sysAwait( EVENT_10MS_TICK );
        sysSend( id.value, &envelope, &envelope );
        assert( envelope.type == MESSAGE_CLOCKSERVER_NOTIFY_TICK );
    }
}

void ClockLoResNotifier()
{
    MessageEnvelope envelope;
    TaskID id = { sysPid() };

    envelope.type = MESSAGE_CLOCKSERVER_NOTIFY_LORES;
    for(;;)
    {
        sysAwait( EVENT_150MS_TICK );
        sysSend( id.value, &envelope, &envelope );
        assert( envelope.type == MESSAGE_CLOCKSERVER_NOTIFY_LORES );
    }
}
