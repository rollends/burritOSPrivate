#include "common/common.h"
#include "kernel/kernel.h"
#include "trains/trains.h"
#include "user/trainservers/trainservices.h"
#include "user/trainservers/TrackManager.h"
#include "user/services/services.h"
#include "user/messageTypes.h"

S32 trainAllocateTrack(U8 trainId, ListU32Node* entryNode)
{
    MessageEnvelope env;
    env.type = 1;
    env.message.MessageArbitrary.body = (U32*)entryNode;
    sysSend(nsWhoIs(TrackManager).value, &env, &env);

    if( env.type == MESSAGE_FAILURE )
    {
        ListU32Node* nFail = (ListU32Node*)env.message.MessageArbitrary.body;
        return -((S32)(nFail->data & 0xFF));
    }
    return 0;
}

U8 trainWhoOwnsTrack(U8 nodeIndex)
{
    MessageEnvelope env;
    env.type = 3;
    env.message.MessageU8.body = nodeIndex;
    sysSend(nsWhoIs(TrackManager).value, &env, &env);
    return env.message.MessageU8.body;
}

static U8 ownershipGraph[TRACK_MAX / 2];

void NodeAttributionServer(void)
{
    nsRegister(NodeAttribution);

    for (;;)
    {
        TaskID from;
        MessageEnvelope env;

        sysReceive(&from.value, &env);
        U8 node = env.message.MessageU8.body;
        env.message.MessageU8.body = ownershipGraph[node/2];
        sysReply(from.value, &env);
    }
}

void TrackManagerServer(void)
{
    U8 i = 0;
    memset(ownershipGraph, 0x00, sizeof(U8) * TRACK_MAX / 2);

    nsRegister(TrackManager);
    sysCreate(sysPriority() + 1, &NodeAttributionServer);

    for(;;)
    {
        TaskID from;
        MessageEnvelope env;

        sysReceive(&from.value, &env);
        
        switch(env.type)
        {
        case 3:
        {
            env.message.MessageU8.body = ownershipGraph[env.message.MessageU8.body / 2];
            sysReply(from.value, &env);
            break;
        }

        case 1:
        {
            ListU32Node* ir = (ListU32Node*)env.message.MessageArbitrary.body;
            ListU32Node* ir2 = ir;
            
            U8 trainId = ir2->data >> 8;
            // Pass 1 : Check if any conflicts. If any , don't allocate! 
            do
            {
                U8 indNode = ir2->data & 0xFF;
                if((ownershipGraph[indNode] != trainId) && (ownershipGraph[indNode] != 0x00))
                {
                    // CONFLICT
                    env.type = MESSAGE_FAILURE;
                    env.message.MessageArbitrary.body = (U32*)ir2; // Failed on this track request :'(
                    sysReply(from.value, &env);
                    break;
                }

                ir2 = ir2->next;
            } while( ir2 );
            
            if( ir2 != 0 )
            {
                break;
            }

            // Revoke any other requests.
            for(i = 0; i < (TRACK_MAX/2); ++i)
                if( ownershipGraph[i] == trainId )
                    ownershipGraph[i] = 0x00;
            
            // Pass 2 : Allocate Track
            do
            {
                U8 indNode = ir->data & 0xFF;
                ownershipGraph[indNode] = trainId;
                ir = ir->next;
            } while( ir );
            
            // Reply
            sysReply(from.value, &env);
            break;
        }

        case 2:
        {
            env.message.MessageU32.body = (U32)(ownershipGraph);
            sysReply(from.value, &env);
            break;
        }

        default:
            assert(env.type == 1);
            break;
        }
    }
}

