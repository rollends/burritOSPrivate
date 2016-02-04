#include "kernel/kernel.h"
#include "user/Nameserver.h"
#include "user/messageTypes.h"

void ClockNotifier()
{
    MessageEnvelope envelope;
    TaskID id;
    nsWhoIs( Clock, &id );

    envelope.type = MESSAGE_CLOCKSERVER_NOTIFY_TICK;
    while( sysRunning() != 0 )
    {
        sysAwaitEvent( EVENT_10MS_TICK );
        sysSend( id.value, &envelope, &envelope );
    }

    envelope.type = MESSAGE_CLOCKSERVER_KILL;
    sysSend( id.value, &envelope, &envelope );
}
