#include "kernel/kernel.h"

#include "user/messageTypes.h"
#include "user/NameServer.h"
#include "user/TerminalInputDriver.h"

void TerminalInputNotifier(void);

void TerminalInputDriver(void)
{
    nsRegister(TerminalInput);
    sysCreate(0, &TerminalInputNotifier);

    // Create Input Buffer
    U8 inputBufferBacking[32];
    QueueU8 inputBuffer;
    queueU8Init( &inputBuffer, inputBufferBacking, 32 );

    // Create Receiver Buffer...really should not be used extensively...
    U16 receiverBacking[32];
    QueueU16 receivers;
    queueU16Init( &receivers, receiverBacking, 32 );

    MessageEnvelope rcvMessage;
    TaskID rcvID;
    for(;;)
    {
        sysReceive(&rcvID.value, &rcvMessage);
        switch( rcvMessage.type )
        {
        case MESSAGE_IO_GETC:
        {
            U8 value;
            if( queueU8Pop( &inputBuffer, &value ) < 0 )
            {
                queueU16Push( &receivers, rcvID.value );
            }
            else
            {
                rcvMessage.message.MessageU8.body = value;
                sysReply( rcvID.value, &rcvMessage );
            }
            break;
        }
        
        case MESSAGE_SIG_KILL:
        {
            sysReply( rcvID.value, &rcvMessage );
            return;
        }

        case DRIVER_MESSAGE_RCV_CHAR:
        {
            sysReply( rcvID.value, &rcvMessage );
            TaskID sendTo;
            if( queueU16Pop( &receivers, &sendTo.value ) >= 0 )
            {
                rcvMessage.type = MESSAGE_IO_GETC;
                sysReply( sendTo.value, &rcvMessage );
            }
            else
            {
                queueU8Push( &inputBuffer, rcvMessage.message.MessageU8.body );
            }
            break;
        }
        }
    }
}

void TerminalInputNotifier(void)
{
    TaskID server;
    MessageEnvelope notif;
    notif.type = DRIVER_MESSAGE_RCV_CHAR;

    server.value = sysPid();
    while( sysRunning() != 0 )
    {
        notif.message.MessageU8.body = sysRead(EVENT_TERMINAL_READ);
        sysSend( server.value, &notif, &notif ); 
    }

    notif.type = MESSAGE_SIG_KILL; 
    sysSend( server.value, &notif, &notif );
}
