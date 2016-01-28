#include "common/types.h"
#include "kernel/message.h"
#include "kernel/sysCall.h"
#include "user/Nameserver.h"
#include "common/queue.h"

S32 nsRegister( TaskName name )
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_NAMESERVER_REGISTER;
    envelope.message.NameserverRequest.name = name;
    return sysSend(2, &envelope, &envelope);
}

S32 nsWhoIs( TaskName name, TaskID* sid )
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_NAMESERVER_WHOIS;
    envelope.message.NameserverRequest.name = name; 
    sysSend( 2, &envelope, &envelope );

    switch( envelope.type )
    {
    case MESSAGE_NAMESERVER_RESPONSE:
    {
        sid->value = envelope.message.NameserverResponse.id;
        return 0;
    }
    default:
        return -1;
    }
}


void Nameserver()
{
    MessageEnvelope envelope;
    U16             nameTable[ TaskNameCount ];
    TaskID          fTid;

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
                    nameTable[envelope.message.NameserverRequest.name] = fTid.value;
                    envelope.message.NameserverResponse.id = fTid.value;
                    gotRegistered = 1;
                    break;

                case MESSAGE_NAMESERVER_WHOIS:
                    queueU16Push( &whoIsClients, fTid.value );
                    queueU8Push( &whoIsRequests, envelope.message.NameserverRequest.name );
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

            envelope.message.NameserverResponse.id = nameTable[name];
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
            nameTable[envelope.message.NameserverRequest.name] = fTid.value;
            envelope.message.NameserverResponse.id = fTid.value;
            break;

        case MESSAGE_NAMESERVER_WHOIS:
            envelope.message.NameserverResponse.id = nameTable[ envelope.message.NameserverRequest.name ];
            break;

        default:
            continue;
        }
        envelope.type = MESSAGE_NAMESERVER_RESPONSE;
        sysReply( fTid.value, &envelope );
    }
    sysExit();
}
