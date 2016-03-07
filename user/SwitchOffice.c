#include "kernel/kernel.h"
#include "user/trackData.h"
#include "user/services/services.h"
#include "user/SwitchOffice.h"
#include "user/messageTypes.h"

typedef struct IntervalNode
{
    SwitchRequest           request;
    struct IntervalNode *   next;
} IntervalNode;

static void SwitchWorker(void)
{
    TaskID task;
    MessageEnvelope env;
    
    sysReceive(&task.value, &env);
    SwitchRequest request = *(SwitchRequest*)(env.message.MessageArbitrary.body);
    sysReply(task.value, &env);

    TaskID clock = nsWhoIs(Clock);
    TaskID sw = nsWhoIs(TrainSwitches);
    clockDelayUntil(clock, request.startTime);
    trainSwitch(sw, request.branchId, request.direction);
    clockDelayUntil(clock, request.endTime);

    sysSend(sysPid(), &env, &env);
}
/*
static void SwitchReceptionist(void)
{
    TaskID phone = VAL_TO_ID( sysPid() );
    TaskID task;

    MessageEnvelope env;
    U8 phoneWaiting = 0;

    for(;;)
    {
        sysReceive(&task.value, &env);
        switch(env.type)
        {
        case MESSAGE_COURIER:
        {
            
            break;
        }

        }
    }
}

static void SwitchExecutivePhone(void)
{
    assert(sysPriority() >= 1);
    
    MessageEnvelope env;
    
    // Create The Receptionist
    TaskID reception = VAL_TO_ID( sysCreate(sysPriority() - 1, &SwitchReceptionist) );
    TaskID executive = VAL_TO_ID( sysPid() );
    for(;;)
    {
        sysSend(reception.value, &env, &env);
        sysSend(executive.value, &env, &env);
    }
}
*/

void SwitchExecutive(void)
{
    U16 priority = sysPriority();
     
    assert(priority >= 1);

    //TaskID phone = VAL_TO_ID( sysCreate(priority - 1, &SwitchExecutivePhone) );

    U8 i = 0;

    TrackNode graph[TRACK_MAX];
    IntervalNode switchIntervalNodes[32];
    IntervalNode *switchFreeList = switchIntervalNodes;
    IntervalNode *switchCalendar[22];

    init_tracka(graph);

    for(i = 0; i < 31; ++i)
        switchIntervalNodes[i].next = switchIntervalNodes + i + 1;
    switchIntervalNodes[i].next = 0;
    
    for(i = 0; i < 22; ++i)
        switchCalendar[i] = 0;

    nsRegister(TrainSwitchOffice);
    for(;;)
    {
        TaskID person;
        MessageEnvelope env;
        sysReceive(&person.value, &env);

        switch(env.type)
        {
        case MESSAGE_WORKER:
        {
            // Pop Node ( SHOULD ALWAYS BE HEAD! )
            SwitchRequest* request = (SwitchRequest*)env.message.MessageArbitrary.body;
            U16 cid = request->indBranchNode - 80; // Get Calendar Index
            IntervalNode* cNode = switchCalendar[cid];
            
            switchCalendar[cid] = cNode->next;

            // Did we pop the right node in this calendar? otherwise we ****ed.
            assert(cNode->request.startTime == request->startTime);

            cNode->next = switchFreeList;
            switchFreeList = cNode;
            sysReply(person.value, &env);
            break;
        }

        default:
        {
            //assert(phone.value == person.value);
            
            SwitchRequest* request = (SwitchRequest*)env.message.MessageArbitrary.body;
            U16 cid = request->indBranchNode - 80; // Get Calendar Index
            IntervalNode* cNode = switchCalendar[cid];
            
            if(!cNode)
            {
                assert(switchFreeList);
                IntervalNode* newInterval = switchFreeList;
                switchFreeList = switchFreeList->next;

                newInterval->request = *request;
                newInterval->next = 0;
                switchCalendar[cid] = newInterval;
                    
                sysReply(person.value, &env);
                env.message.MessageArbitrary.body = (U32*)(&newInterval->request);
                sysSend(sysCreate(priority - 1, &SwitchWorker), &env, &env);    
                break;
            }

            SwitchRequest cRequest = cNode->request;
            IntervalNode* next = cNode->next;
            for(;;)
            {
                if( ((cRequest.startTime <= request->endTime) && (cRequest.endTime >= request->startTime))
                 || ((cRequest.endTime <= request->startTime) && (cRequest.startTime >= request->endTime)))
                {
                    // Fail! The switch is taken!
                    assert(0);
                }


                if(!next || (next && (next->request.startTime >= request->endTime)))
                {
                    break;
                }
                cNode = next;
            }

            assert(switchFreeList);
            IntervalNode* newInterval = switchFreeList;
            switchFreeList = switchFreeList->next;

            newInterval->request = *request;
            cNode->next = newInterval;
            newInterval->next = next;
                    
            sysReply(person.value, &env);
            env.message.MessageArbitrary.body = (U32*)(&newInterval->request);
            sysSend(sysCreate(priority - 1, &SwitchWorker), &env, &env);
            break;
        }
        }
    }
}
