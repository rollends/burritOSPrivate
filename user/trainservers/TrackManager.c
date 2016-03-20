#include "common/common.h"
#include "kernel/kernel.h"
#include "trains/trains.h"
#include "user/trainservers/trainservices.h"
#include "user/trainservers/TrackManager.h"
#include "user/services/services.h"
#include "user/messageTypes.h"

S32 trainAllocateTrack(U8 trainId, TrackRequest* entryNode)
{
    MessageEnvelope env;
    env.type = 1;
    env.message.MessageArbitrary.body = (U32*)entryNode;
    sysSend(nsWhoIs(TrackManager).value, &env, &env);

    if( env.type == MESSAGE_FAILURE ) 
        return -1;

    return 0;
}

void TrackManagerServer(void)
{
    //TrackNode graph[TRACK_MAX];
    //init_trackb(graph);
    U8 i = 0;
    U16 ownershipGraph[TRACK_MAX / 2];
    memset(ownershipGraph, 0xFF, sizeof(U16) * TRACK_MAX / 2);

    nsRegister(TrackManager);
    for(;;)
    {
        TaskID from;
        MessageEnvelope env;

        sysReceive(&from.value, &env);
        
        switch(env.type)
        {
        case 1:
        {
            TrackRequest* ir = (TrackRequest*)env.message.MessageArbitrary.body;
            
            // Revoke any other requests.
            for(i = 0; i < (TRACK_MAX/2); ++i)
                if( ownershipGraph[i] == ir->trainId )
                    ownershipGraph[i] = 0xFFFF;
            
            // Allocate the request.
            do
            {
                if((ownershipGraph[ir->indNode] != ir->trainId) && (ownershipGraph[ir->indNode] != 0xFFFF))
                {
                    // CONFLICT
                    env.type = MESSAGE_FAILURE;
                    env.message.MessageArbitrary.body = (U32*)ir; // Failed on this track request :'(
                    break;
                }

                ownershipGraph[ir->indNode] = ir->trainId;
                ir = ir->pForwardRequest;
            } while( ir );
            
            // Reply
            sysReply(from.value, &env);
            break;
        }

        default:
            assert(env.type == 1);
            break;
        }
    }
}

