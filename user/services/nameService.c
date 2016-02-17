#include "kernel/kernel.h"
#include "user/messageTypes.h"
#include "user/services/nameService.h"

S32 nsRegister( TaskName name )
{
    MessageEnvelope envelope;
    envelope.type = MESSAGE_NAMESERVER_REGISTER;
    envelope.message.MessageU8.body = name;
    return sysSend(2, &envelope, &envelope);
}

TaskID nsWhoIs( TaskName name )
{
    TaskID result;

    MessageEnvelope envelope;
    envelope.type = MESSAGE_NAMESERVER_WHOIS;
    envelope.message.MessageU8.body = name; 
    sysSend( 2, &envelope, &envelope );
    
    assert( envelope.type == MESSAGE_NAMESERVER_RESPONSE );

    result.value = envelope.message.MessageU16.body;
    return result;
}



