#ifndef MESSAGE_H
#define MESSAGE_H

#include "common/common.h"

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

        /**
         * Word Message
         */
        struct
        {
            U32         body;
        } MessageU32;

        /**
         * Arbitrary sized Message
         */
        struct
        {
            U32*        body;
        } MessageArbitrary;
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
