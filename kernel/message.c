#include "kernel/message.h"

S32 messageCopy(MessageEnvelope* dst, MessageEnvelope const * src)
{
    S32 returnVal = 0;

    switch (src->type)
    {
        case MESSAGE_NAMESERVER_WHOIS:
        case MESSAGE_NAMESERVER_REGISTER:
        case MESSAGE_RPS:
        case MESSAGE_RANDOM_BYTE:
            dst->message.MessageU8 = src->message.MessageU8;
            break;

        case MESSAGE_NAMESERVER_RESPONSE:
            dst->message.MessageU16 = src->message.MessageU16;
            break;

        default:
            returnVal = -1;
            break;
    }

    dst->type = src->type;
    return returnVal;
}
