#include "kernel/kernel.h"
#include "trains/trains.h"

#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"

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
    env.type = MESSAGE_WORKER;
    SwitchRequest request = *(SwitchRequest*)(env.message.MessageArbitrary.body);
    sysReply(task.value, &env);

    TaskID clock = nsWhoIs(Clock);
    TaskID sw = nsWhoIs(TrainSwitches);
    clockDelayUntil(clock, request.startTime);
    trainSwitch(sw, request.branchId, request.direction);
    
    sysSend(sysPid(), &env, &env);
    
    clockDelayUntil(clock, request.endTime);
}

static U32 indexForBranch(TrackNode* graph, U32 branchId)
{
    U32 i = 0;
    for(i = 0; i < TRACK_MAX; ++i)
    {
        if( graph[i].type == eNodeBranch && graph[i].num == branchId )
            return (i - 80)/2;
    }
    return 0xFFFFFFFF;
}

void SwitchExecutive(void)
{
    U16 priority = sysPriority();
     
    assert(priority >= 1);

    U8 i = 0;

    TrackNode graph[TRACK_MAX];
    init_tracka(graph);

    IntervalNode switchIntervalNodes[32];
    IntervalNode *switchFreeList = switchIntervalNodes;
    IntervalNode *switchCalendar[22];
    SwitchState switches[22];


    for(i = 0; i < 31; ++i)
        switchIntervalNodes[i].next = switchIntervalNodes + i + 1;
    switchIntervalNodes[i].next = 0;
    
    for(i = 0; i < 22; ++i)
    {
        switchCalendar[i] = 0;
        switches[i] = eCurved;
    }
    switches[6] = eStraight;
    switches[7] = eStraight;
    switches[13] = eStraight;
    switches[19] = eStraight;
    switches[21] = eStraight;

    nsRegister(TrainSwitchOffice);

    for(i = 80; i < 123 ; i += 2)
    {
        TrackNode* iSwitch = graph + i;
        trainSwitch(nsWhoIs(TrainSwitches), iSwitch->num, switches[(i-80)/2]);
    }

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
            U32 cid = indexForBranch(graph, request->branchId); // Get Calendar Index
            assert(cid <= 21);
            IntervalNode* cNode = switchCalendar[cid];
            
            switchCalendar[cid] = cNode->next;

            // Did we pop the right node in this calendar? otherwise we ****ed.
            assert(cNode->request.startTime == request->startTime);

            cNode->next = switchFreeList;
            switchFreeList = cNode;
            switches[cid] = request->direction;
            
            sysReply(person.value, &env);
            break;
        }

        case MESSAGE_SWITCH_READ:
        {
            assert(env.message.MessageU32.body <= 21);
            env.message.MessageU32.body = (U32)switches[env.message.MessageU32.body];
            sysReply(person.value, &env);
            break;
        }

        case MESSAGE_SWITCH_ALLOCATE:
        {
            
            SwitchRequest* request = (SwitchRequest*)env.message.MessageArbitrary.body;
            U32 cid = indexForBranch(graph,request->branchId); // Get Calendar Index
            assert(cid <= 21);
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
