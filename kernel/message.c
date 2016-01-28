#include "kernel/message.h"

S32 messageCopy(MessageEnvelope* dst, MessageEnvelope const * src)
{
    S32 returnVal = 0;

    switch (src->type)
    {
        case MESSAGE_NAMESERVER_WHOIS:
        case MESSAGE_NAMESERVER_REGISTER:
            dst->message.NameserverRequest = src->message.NameserverRequest;
            break;

        case MESSAGE_NAMESERVER_RESPONSE:
            dst->message.NameserverResponse = src->message.NameserverResponse;
            break;

        case MESSAGE_RPS:
            dst->message.RPSMessage = src->message.RPSMessage;
            break;
 
        case MESSAGE_RANDOM_BYTE:
            dst->message.RandomSeed = src->message.RandomSeed;
            break;

        default:
            returnVal = -1;
            break;
    }

    dst->type = src->type;
    return returnVal;
}
