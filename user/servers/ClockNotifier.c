#include "kernel/kernel.h"
#include "user/messageTypes.h"

void ClockNotifier()
{
    MessageEnvelope envelope;
    TaskID id;
    id.value = sysPid();

    envelope.type = MESSAGE_CLOCKSERVER_NOTIFY_TICK;
    while( sysRunning() != 0 )
    {
        sysAwait( EVENT_10MS_TICK );
        sysSend( id.value, &envelope, &envelope );
        assert( envelope.type == MESSAGE_CLOCKSERVER_NOTIFY_TICK );
    }
}
