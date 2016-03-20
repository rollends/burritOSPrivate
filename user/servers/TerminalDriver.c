#include "common/string.h"

#include "user/messageTypes.h"
#include "user/services/services.h"

static void TermOutNotif(void);
static void TermInNotif(void);

void TerminalDriver(void)
{
    nsRegister(Terminal);
    assert(sysPriority() >= 1);
    TaskID      inputId      = { sysCreate(sysPriority() - 1, &TermInNotif) },
                outputId     = { sysCreate(sysPriority() - 1, &TermOutNotif) };

    U16 const   inputBufferLength           = 32,
                outputBufferLength          = 4096;
    U8          inputBuffer[inputBufferLength];
    U8          outputBuffer[outputBufferLength];
    QueueU8     qInput;
    U16         outputHead                  = 0,
                outputTail                  = 0;
    U32         outputCount                 = 0;
    U16         getcTask                    = 0;
    U8          isOutputNotifierReady       = 0;

    queueU8Init(&qInput, inputBuffer, inputBufferLength);

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
            if( queueU8Pop( &qInput, &value ) < 0 )
            {
                assert( getcTask == 0 );
                getcTask = rcvID.value;
            }
            else
            {
                rcvMessage.message.MessageU8.body = value;
                sysReply( rcvID.value, &rcvMessage );
            }
            break;
        }

        case MESSAGE_IO_PUTSTR:
        {
            ConstString str = (ConstString)rcvMessage.message.MessageArbitrary.body;
            U32 length = strlen(str);

            assert( (outputCount + length) < outputBufferLength ); // Buffer overflow!

            U32 i = 0;
            for(i = 0; i < length; ++i)
            {
                outputBuffer[outputTail++] = str[i];
                outputTail &= (outputBufferLength - 1);
            }
            outputCount += length;
 
            // Reply now that we've copied.
            sysReply(rcvID.value, &rcvMessage);
            
            if( isOutputNotifierReady )
            {
                // Dequeue character.
                rcvMessage.message.MessageU8.body = outputBuffer[outputHead++];
                outputHead &= (outputBufferLength - 1);

                // Reply to the output notifier to push out byte.
                sysReply( outputId.value, &rcvMessage );
                isOutputNotifierReady = 0;
                outputCount--;
            }
            break;
        }

        case DRIVER_MESSAGE_TX_CHAR:
        {
            if( outputHead != outputTail )
            {
                // Dequeue character.
                rcvMessage.message.MessageU8.body = outputBuffer[outputHead++];
                outputHead &= (outputBufferLength - 1);
                outputCount--;
                sysReply(rcvID.value, &rcvMessage);
            }
            else
            {
                isOutputNotifierReady = 1;
            }
            break;
        }

        case DRIVER_MESSAGE_RCV_CHAR:
        {
            assert( inputId.value == rcvID.value );
            sysReply( inputId.value, &rcvMessage );
            if( getcTask > 0 )
            {
                rcvMessage.type = MESSAGE_IO_GETC;
                sysReply( getcTask, &rcvMessage );
                getcTask = 0;
            }
            else
            {
                queueU8Push( &qInput, rcvMessage.message.MessageU8.body );
            }
            break;
        }
        }
    }
}

static void TermOutNotif(void)
{
    TaskID server = { sysPid() };
    MessageEnvelope notif;
    for(;;)
    {
        notif.type = DRIVER_MESSAGE_TX_CHAR;
        sysSend( server.value, &notif, &notif );
        sysWrite( EVENT_TERMINAL_WRITE, notif.message.MessageU8.body );
    }
}

static void TermInNotif(void)
{
    TaskID server = { sysPid() };
    MessageEnvelope notif;
    for(;;) 
    {
        notif.type = DRIVER_MESSAGE_RCV_CHAR;
        notif.message.MessageU8.body = sysRead(EVENT_TERMINAL_READ);
        sysSend( server.value, &notif, &notif ); 
    }
}
