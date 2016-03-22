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

static TrackNode switchServerGraph[TRACK_MAX];
static volatile SwitchState switches[22];

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
  
    U8 cid = indexForBranch(switchServerGraph, request.branchId);
    switches[cid] = request.direction;

    if((request.branchId == 156) && (request.direction == eCurved))
    {
        cid = indexForBranch(switchServerGraph, 155);
        trainSwitch(sw, 155, eStraight);
        switches[cid] = eStraight;
    }
    else if((request.branchId == 154) && (request.direction == eCurved))
    {
        cid = indexForBranch(switchServerGraph, 153);
        trainSwitch(sw, 153, eStraight);
        switches[cid] = eStraight;
    }

    sysSend(sysPid(), &env, &env);
    clockDelayUntil(clock, request.endTime);
}

void SwitchExecutive(void)
{
    U16 priority = sysPriority();
     
    assert(priority >= 1);

    U8 i = 0;

    init_tracka(switchServerGraph);

    IntervalNode switchIntervalNodes[32];
    IntervalNode *switchFreeList = switchIntervalNodes;
    IntervalNode *switchCalendar[22];


    for(i = 0; i < 31; ++i)
        switchIntervalNodes[i].next = switchIntervalNodes + i + 1;
    switchIntervalNodes[i].next = 0;
    
    for(i = 0; i < 22; ++i)
    {
        switchCalendar[i] = 0;
        switches[i] = eCurved;
    }
    switches[3] = eStraight;
    switches[6] = eStraight;
    switches[7] = eStraight;
    switches[8] = eStraight;
    switches[10] = eStraight;
    switches[19] = eStraight;
    switches[21] = eStraight;

    nsRegister(TrainSwitchOffice);

    for(i = 80; i < 123 ; i += 2)
    {
        TrackNode* iSwitch = switchServerGraph + i;
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
            U32 cid = indexForBranch(switchServerGraph, request->branchId); // Get Calendar Index
            assert(cid <= 21);
            IntervalNode* cNode = switchCalendar[cid];
            
            switchCalendar[cid] = cNode->next;

            // Did we pop the right node in this calendar? otherwise we ****ed.
            assert(cNode->request.startTime == request->startTime);

            cNode->next = switchFreeList;
            switchFreeList = cNode;
            
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
            U32 cid = indexForBranch(switchServerGraph,request->branchId); // Get Calendar Index
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
                
                // First one in...set it.
                switches[cid] = request->direction;
                
                sysReply(person.value, &env);
                env.message.MessageArbitrary.body = (U32*)(&newInterval->request);
                sysSend(sysCreate(priority - 1, &SwitchWorker), &env, &env);    
                break;
            }

            SwitchRequest cRequest = cNode->request;
            IntervalNode* next = cNode->next;
            U8 taken = 0;
            while(!taken)
            {
                if( ((cRequest.startTime <= request->endTime) && (cRequest.endTime >= request->startTime))
                 || ((cRequest.endTime <= request->startTime) && (cRequest.startTime >= request->endTime)))
                {
                    // Fail! The switch is taken!
                    sysReply(person.value, &env);
                    taken = 1;
                    //assert(0);
                }


                if(!next || (next && (next->request.startTime >= request->endTime)))
                {
                    break;
                }
                cNode = next;
            }

            if( taken )
                break;

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
