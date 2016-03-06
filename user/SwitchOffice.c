#include "kernel/kernel.h"

typedef struct IntervalNode;
{
    SwitchRequest           request;
    struct IntervalNode *   next;
} IntervalNode;

static void SwitchWorker(void)
{
    TaskID executive = VAL_TO_ID( sysPid() ); 
}

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

void SwitchExecutive(void)
{
    U16 priority = sysPriority();
     
    assert(priority >= 1);

    TaskID phone = VAL_TO_ID( sysCreate(priority - 1, &SwitchExecutivePhone) );
    TaskID worker = VAL_TO_ID( sysCreate(priority - 1, &SwitchWorker) );

    MessageEnvelope courierEnvelope;
    U8 i = 0;

    TrackNode graph[TRACK_MAX];
    IntervalNode switchIntervalNodes[32];
    IntervalNode switchFreeList = switchIntervalNodes;
    IntervalNode *switchCalendar[22];

    init_tracka(graph);

    for(i = 0; i < 31; ++i)
        switchRequestNodes[i].next = switchRequestNodes + i + 1;
    switchRequestNodes[i].next = 0;
    
    for(i = 0; i < 22; ++i)
        switchCalendar[i] = 0;

    for(;;)
    {
        TaskID person;
        MessageEnvelope env;
        sysReceive(&person.value, &env);

        switch(env.type)
        {
        case MESSAGE_WORKER:
        {
            assert(worker.value == person.value);
           
            break;
        }

        default:
        {
            assert(phone.value == person.value);
            
            SwitchRequest* request = env.message.MessageArbitrary.body;
            U16 cid = request->indBranchNode - 80; // Get Calendar Index
            IntervalNode* cNode = switchCalendar[cid];
            while(cNode)
            {
                SwitchRequest cRequest = cNode->request;
                if( ((cRequest.startTime <= request->endTime) && (cRequest.endTime >= request->startTime))
                 || ((cRequest.endTime <= request->startTime) && (cRequest.startTime >= request->endTime)))
                {
                    // Fail! The switch is taken!
                    assert(0);
                }

                IntervalNode* next = cNode->next;

                if(!next || (next && (next->request.startTime >= request->endTime)))
                {
                    assert(switchFreeList);
                    IntervalNode* newInterval = switchFreeList;
                    switchFreeList = switchFreeList->next;

                    newInterval->request = *request;
                    cNode->next = newInterval;
                    newInterval->next = next;
                    break;
                }
                cNode = next;
            }
            
            
            break;
        }
        }
    }
}
