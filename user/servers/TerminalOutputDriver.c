#include "common/string.h"

#include "kernel/kernel.h"

#include "user/messageTypes.h"
#include "user/servers/TerminalOutputDriver.h"
#include "user/services/nameService.h"

void TerminalOutputWarehouse(void);
void TerminalOutputCourier(void);
void TerminalOutputNotifier(void);

void TerminalOutputCourier(void)
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_COURIER;
    
    TaskID warehouse; 
    TaskID server;
    
    warehouse.value = sysCreate(3, &TerminalOutputWarehouse);
    server.value = sysPid();
    
    for(;;)
    {
        sysSend( warehouse.value, &envelope, &envelope );
        sysSend( server.value, &envelope, &envelope );
    }
}

void TerminalOutputWarehouse(void)
{
    nsRegister(TerminalOutput);

    U32 const bufferLength = 2048;
    U8 bufferBacking[2048];
    QueueU8 buffer;
    queueU8Init( &buffer, bufferBacking, bufferLength );

    U8 blockedTaskValueBacking[32];
    U16 blockedTaskBacking[32];
    QueueU16 blockedTasks;
    QueueU8 blockedTaskValue;

    queueU16Init( &blockedTasks, blockedTaskBacking, 32 );
    queueU8Init( &blockedTaskValue, blockedTaskValueBacking, 32 );

    TaskID courierID;
    U8 isCourierWaiting = 0;

    courierID.value = sysPid();

    MessageEnvelope envelope;
    TaskID id;
    for(;;)
    {
        sysReceive( &id.value, &envelope );
        switch( envelope.type )
        {
        case MESSAGE_IO_PUTSTR:
        {
            char const * str = (char const *)envelope.message.MessageArbitrary.body;
            
            assert( (buffer.count + strlen( str )) < bufferLength );
            
            if( isCourierWaiting )
            {
                envelope.type = MESSAGE_COURIER;
                envelope.message.MessageU8.body = *(str++);
                sysReply( courierID.value, &envelope );
                isCourierWaiting = 0;
            }

            char c;
            while( (c = *(str++)) )
            {
                queueU8Push(&buffer, c); 
            }

            envelope.type = MESSAGE_IO_PUTSTR;
            envelope.message.MessageU32.body = 0;
            sysReply(id.value, &envelope);
            break;
        }

        case MESSAGE_IO_PUTC:
        {
            U8 c = envelope.message.MessageU8.body;
            if( isCourierWaiting )
            {
                envelope.type = MESSAGE_COURIER;
                sysReply( courierID.value, &envelope );

                isCourierWaiting = 0;
                envelope.type = MESSAGE_IO_PUTC;
                sysReply( id.value, &envelope );
            }
            else if( queueU8Push( &buffer, c ) >= 0 )
            {
                // Success
                sysReply( id.value, &envelope ); 
            }
            else
            {
                queueU16Push( &blockedTasks, id.value );
                queueU8Push( &blockedTaskValue, c );
            }
           break;
        }

        case MESSAGE_COURIER:
        {
            if( queueU8Pop( &buffer, &envelope.message.MessageU8.body ) >= 0 )
            {
                sysReply( id.value, &envelope );
            
                if( queueU8Pop( &blockedTaskValue, &envelope.message.MessageU8.body ) >= 0 )
                {
                    U16 task;
                    queueU16Pop( &blockedTasks, &task );
                    queueU8Push( &buffer, envelope.message.MessageU8.body );

                    envelope.type = MESSAGE_IO_PUTC;
                    sysReply( task, &envelope );
                }
            }
            else
            {
                isCourierWaiting = 1;
            }
            break;
        }
        }
    }
}

void TerminalOutputDriver(void)
{
    TaskID notifier;
    TaskID courier; 
    
    notifier.value = sysCreate(0, &TerminalOutputNotifier);
    courier.value = sysCreate(2, &TerminalOutputCourier);

    U8 isNotifierReady = 0;
    U8 isCourierReady = 0;
    U8 extraBuffer = 0;

    U8 bufferBacking[1024];
    QueueU8 buffer;
    queueU8Init(&buffer, bufferBacking, 1024);

    MessageEnvelope rcvMessage;
    TaskID rcvID;
    for(;;)
    {
        sysReceive(&rcvID.value, &rcvMessage);
        switch( rcvMessage.type )
        {
        case MESSAGE_COURIER:
        {
            if( isNotifierReady )
            {
                isNotifierReady = 0;
                rcvMessage.type = DRIVER_MESSAGE_TX_CHAR;
                sysReply( notifier.value, &rcvMessage );                
                
                rcvMessage.type = MESSAGE_COURIER;
                sysReply( rcvID.value, &rcvMessage );
            }
            else if( queueU8Push(&buffer, rcvMessage.message.MessageU8.body) >= 0 )
            {
                sysReply( rcvID.value, &rcvMessage );
            }
            else
            {
                extraBuffer = rcvMessage.message.MessageU8.body;
                isCourierReady = 1;
            }
            break;
        }

        case DRIVER_MESSAGE_TX_CHAR:
        {
            if( queueU8Pop(&buffer, &rcvMessage.message.MessageU8.body) >= 0 )
            {
                sysReply(rcvID.value, &rcvMessage);

                if( isCourierReady )
                {
                    queueU8Push(&buffer, extraBuffer);

                    rcvMessage.type = MESSAGE_COURIER;
                    rcvMessage.message.MessageU8.body = extraBuffer;
                    sysReply(courier.value, &rcvMessage);
                    isCourierReady = 0;
                }
            }
            else
            {
                isNotifierReady = 1;
            }
            break;
        }
        }
    }
}

void TerminalOutputNotifier(void)
{
    TaskID server;
    
    server.value = sysPid();

    MessageEnvelope notif;
    notif.type = DRIVER_MESSAGE_TX_CHAR;
    for(;;)
    {
        sysSend( server.value, &notif, &notif );
        sysWrite( EVENT_TERMINAL_WRITE, notif.message.MessageU8.body );
    }
}


