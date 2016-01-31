#ifndef MESSAGE_H
#define MESSAGE_H

#include "common/types.h"

#define     MESSAGE_NULL                        0
#define     MESSAGE_NAMESERVER_WHOIS            1
#define     MESSAGE_NAMESERVER_REGISTER         2
#define     MESSAGE_NAMESERVER_RESPONSE         3
#define     MESSAGE_RPS                         4
#define     MESSAGE_RANDOM_BYTE                 5

/**
 * Message holder struct that includes the message type and a union of 
 * possible message types
 */
typedef struct
{
    /// The message type
    U8 type;

    /// The message body union
    union
    {
        /**
         * Byte-sized Message (yum!)
         */
        struct
        {
            U8          body;
        } MessageU8;

        /**
         * Half-word Message
         */
        struct
        {
            U16         body;
        } MessageU16;
    } message;
} MessageEnvelope;

/**
 * Performs a deep copy of a message envelope
 *
 * @param   dst     The desintation envelope
 * @param   src     The source envelope
 *
 * @return  0 on success, else an error code
 */
static inline S32 messageCopy(MessageEnvelope* dst, MessageEnvelope const * src)
{
    dst->type = src->type;
    dst->message = src->message;
    return 0;
}

#endif
