#ifndef MESSAGE_H
#define MESSAGE_H

#include "common/types.h"

#define     MESSAGE_NULL                        0
#define     MESSAGE_NAMESERVER_WHOIS            1
#define     MESSAGE_NAMESERVER_REGISTER         2
#define     MESSAGE_NAMESERVER_RESPONSE         3

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
         * Struct for requests sent to a name server
         */
        struct
        {
            U8          name;
        } NameserverRequest;

        /**
         * Struct for responses from the name server
         */
        struct
        {
            U16         id;
        } NameserverResponse;
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
S32 messageCopy(MessageEnvelope* dst, MessageEnvelope const * src);

#endif
