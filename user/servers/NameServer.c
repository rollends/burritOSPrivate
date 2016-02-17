#include "kernel/kernel.h"
#include "user/messageTypes.h"
#include "user/servers/NameServer.h"
#include "user/services/nameService.h"

void NameServer()
{
    MessageEnvelope envelope;
    U16             nameTable[ TaskNameCount ];
    TaskID          fTid;

    assert( sysTid() == 2 );

    {
        U16             whoIsClientsBacking[ 32 ];
        U8              whoIsRequestBacking[ 32 ];
        U16             registrationLeft            = TaskNameCount;
        QueueU16        whoIsClients;
        QueueU8         whoIsRequests;

        // Initialize Nameserver Table...
        queueU16Init( &whoIsClients, whoIsClientsBacking, 32 );
        queueU8Init( &whoIsRequests, whoIsRequestBacking, 32 );
        while( registrationLeft )
        {
            U8 gotRegistered = 0;
            while( gotRegistered == 0 )
            {
                sysReceive( &fTid.value, &envelope );
                switch( envelope.type )
                {
                case MESSAGE_NAMESERVER_REGISTER:
                    nameTable[envelope.message.MessageU8.body] = fTid.value;
                    envelope.message.MessageU16.body = fTid.value;
                    gotRegistered = 1;
                    break;

                case MESSAGE_NAMESERVER_WHOIS:
                    queueU16Push( &whoIsClients, fTid.value );
                    queueU8Push( &whoIsRequests, envelope.message.MessageU16.body );
                    break;
                }
            }
            envelope.type = MESSAGE_NAMESERVER_RESPONSE;
            sysReply( fTid.value, &envelope );
            registrationLeft--;
        }

        // Handle queued requests
        U16 id;
        while( queueU16Pop( &whoIsClients, &id ) >= 0  )
        {
            TaskName name;
            U8 nameVal;
            
            queueU8Pop( &whoIsRequests, &nameVal );
            name = nameVal;

            envelope.message.MessageU16.body = nameTable[name];
            envelope.type = MESSAGE_NAMESERVER_RESPONSE;
            sysReply( id, &envelope );
        }
    }

    for(;;)
    {
        sysReceive( &fTid.value, &envelope );

        switch( envelope.type )
        {
        case MESSAGE_NAMESERVER_REGISTER:
            nameTable[envelope.message.MessageU8.body] = fTid.value;
            envelope.message.MessageU16.body = fTid.value;
            break;

        case MESSAGE_NAMESERVER_WHOIS:
            envelope.message.MessageU16.body = nameTable[ envelope.message.MessageU8.body ];
            break;

        default:
            continue;
        }
        envelope.type = MESSAGE_NAMESERVER_RESPONSE;
        sysReply( fTid.value, &envelope );
    }
}
