#include "common/common.h"
#include "kernel/kernel.h"
#include "hardware/hardware.h"
#include "trains/trains.h"
#include "user/display/LogDisplay.h"
#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"
#include "user/trainservers/Locomotive.h"

void locomotiveThrottle (LocomotiveState* state, U8 speed)
{
    state->speed = speed;
    trainPhysicsSetSpeed(&state->physics, state->speed);
    trainSetSpeed(state->sTrainServer, state->train, state->speed);
}

void locomotiveStateInit (LocomotiveState* state, U8 train)
{
    memset(state, 0, sizeof(LocomotiveState));

    init_tracka(state->graph);
    state->sTrainServer = nsWhoIs(Train);
    state->train = train;
    state->sSwitchServer = nsWhoIs(TrainSwitchOffice);

    trainInit(&state->physics, train);
    locomotiveThrottle(state, 0);

    state->isLaunching  = 1;
    state->sensor       = 0;

    state->gotoSensor   = 0xFF;
    state->gotoSpeed    = 0xFF;
    state->random = (train + 1) ^ (0x1a3fb13f);

    state->allocatedDistance = 0;
    state->distToStop = 0;
    state->stopDistance = 0;

    memset(state->predictSensor, 0xFF, sizeof(U32) * 4);
    
    listU32Init(&state->allocatedTrack, state->aAllocatedTrack, 25);
}

S32 locomotiveAllocateTrack (LocomotiveState* state, S32 distanceRequired, QueueU8* qBranchId, QueueU8* qBranchAction)
{
    assert(state->sensor);

    state->distToStop = 0;

    TrackNode* ip = state->sensor;   
    ListU32Node aRequests[25];
    ListU32 requests;
    listU32Init(&requests, aRequests, 25);  

    ListU32Node* node = state->destinationPath.path.head;
    U8 indNext = (node != 0 ? node->data : 0xFF);
 
    U32 distToTravel = 0;
    U32 sensorCount = 0; 
    U32 onPath = 1;
    do
    {
        assert(requests.freeList);
        assert(distanceRequired < 5000);

        if( indNext != 0xFF )
        {
            node = node->next;
            indNext = (node != 0 ? node->data : 0xFF);
        }

        U32 indCurrent = ip - state->graph;
        TrackEdge* edge = &ip->edge[DIR_AHEAD];
        if(ip->type == eNodeBranch)
        {
            SwitchState sw = eStraight;
            MessageEnvelope env;
            env.type = MESSAGE_SWITCH_READ;
            env.message.MessageU32.body = (indCurrent - 80) / 2;
            sysSend(state->sSwitchServer.value, &env, &env);
            sw = (SwitchState)env.message.MessageU32.body;

            if( indNext != 0xFF )
            {
                SwitchState swn = ((ip->edge[DIR_AHEAD].dest - state->graph) == indNext 
                    ? eStraight 
                    : eCurved);
                
                if( swn != sw )
                {
                    if((distanceRequired > 0) || (sensorCount < 3))
                    {
                        if(distToTravel >= state->physics.distance)
                        {
                            S32 kticks = trainPhysicsGetTime(&state->physics, distToTravel - state->physics.distance) / 1000;
                            if ((kticks >= 400) && 
                                ((distToTravel - state->physics.distance) >= 200))
                            {
                                assertOk(queueU8Push(qBranchId, ip->num));
                                assertOk(queueU8Push(qBranchAction, swn));
                                sw = swn;
                            }
                            else
                            {
                                onPath = 0;
                            }
                        }
                        else
                        {
                            onPath = 0;
                        }
                    }
                    else
                    {
                        sw = swn;
                    }
                }
            }
            edge = &ip->edge[(sw == eCurved ? DIR_CURVED : DIR_AHEAD)];  
        }
        else if(ip->type == eNodeSensor)
        {
            sensorCount++;
        }

        if ((distanceRequired > 0) || (sensorCount <= 3))
        {
            listU32PushBack(&requests, (state->train << 8) | (indCurrent / 2));
            distanceRequired -= (edge->dist - edge->dx);
            distToTravel += (edge->dist - edge->dx);
        }

        if (state->shouldStop && ip->num == state->gotoSensor)
            break;

        state->distToStop += (edge->dist - edge->dx);

        if (ip->type == eNodeExit) 
            break;

        ip = edge->dest;
    } while((state->shouldStop && onPath) || (distanceRequired > 0) || (sensorCount < 3));
   
    if(onPath && state->shouldStop)
    {
        state->distToStop -= state->physics.distance;
    }
    else
    {
        state->distToStop = 50000; // 50m lol.
    }

    assert(requests.head);
    S32 result = trainAllocateTrack(state->train, requests.head);

    if(result >= 0)
    {
        state->allocatedDistance = distToTravel;

        listU32Init(&state->allocatedTrack, state->aAllocatedTrack, 25);
        while(requests.head)
        {
            U32 val;
            assertOk(listU32PopFront(&requests, &val));
            assertOk(listU32PushBack(&state->allocatedTrack, val));
        }
    }

    return result;
}

void locomotiveStep (LocomotiveState* state, U32 deltaTime)
{
    if(!state->sensor)
        return;
    
    if (deltaTime)
    {
        trainPhysicsStep(&state->physics, deltaTime);
    }

    if(state->isStopping)
    {
        if(state->physics.speed != 0)
        {
            return;
        }
        else
        {
            state->isStopping = 0;

            // Safety purposes..lets just make sure we stopped
            clockDelayBy(nsWhoIs(Clock), 200);
        }
    }

    state->distanceRequired = min(state->physics.distance, state->predictDistance[1]);
    if(state->physics.targetSpeed)
    {
        state->stopDistance = trainPhysicsStopDist(&state->physics);
    }
    U32 clearance = (3 * state->stopDistance) / 2 + (state->direction ? 200 : 0);
    state->distanceRequired += clearance;

    if (state->hasConflict == 1)
    {
        // Try a reallocation.
        // we already stopped, wait a random amount (doesn't matter if physics tick is delayed. calculations are 
        // not being done for the 0 speed)
        clockDelayBy(nsWhoIs(Clock), 100 * (nextRandU32(&state->random) % 10 + 1));
        state->hasConflict = 2;
    }
    else if (state->gotoSensor < 0xFF)
    {
        U8 aBranchAction[16], aBranchId[16];
        QueueU8 qBranchAction, qBranchId;
        queueU8Init(&qBranchAction, aBranchAction, 16);
        queueU8Init(&qBranchId, aBranchId, 16);

        S32 failedNode = locomotiveAllocateTrack(state, state->distanceRequired, &qBranchId, &qBranchAction);

        if(state->shouldStop && state->stopDistance >= state->distToStop)
        {
            state->isStopping = 1;
            locomotiveThrottle(state, 0);
            logMessage( "[Train %d] Stopping at %c%2d.", 
                        state->train, 
                        'A' + state->gotoSensor/16,
                        state->gotoSensor % 16 + 1);
            state->gotoSensor = 0xFF;
        }
        else if(failedNode >= 0 && state->distanceRequired > state->allocatedDistance && state->distToStop < 50000)
        {
            // STOP!
            logMessage( "[Train %d] Somehow we allocated track but not enough? Stop NOW!", 
                        state->train );
            state->isStopping = 1;
            locomotiveThrottle(state, 0);
        }
        else if(failedNode < 0)
        {
            state->hasConflict = 1;
            
            // Failed allocation
            if (state->speed == 0)
            {
                if( pathFind(state->graph, state->sensor->reverse->num, state->gotoSensor, &state->destinationPath) >= 0 )
                {
                    state->isReversing = ~state->isReversing;
                    state->sensor = state->sensor->reverse;
                    state->physics.distance = 0;
                    locomotiveMakePrediction(state);
                }
                else
                {
                    pathFind(state->graph, state->sensor->num, state->gotoSensor, &state->destinationPath);
                }
            }
            else
            {
                logMessage("[Train %d] Yielding (stopping for failed allocation).", state->train);
                
                state->isStopping = 1;
                locomotiveThrottle(state, 0);
            }
        }
        else
        {
            TaskID clock = nsWhoIs(Clock);

            if(state->hasConflict == 2)
            {
                state->hasConflict = 0;
                if(state->isReversing)
                {
                    MessageEnvelope env;
                    env.type = MESSAGE_TRAIN_REVERSE;
                    env.message.MessageU16.body = (state->train << 8) | 0x0F;
                    sysSend(state->sTrainServer.value, &env, &env);
                    logMessage("[Train %d] Reversing and continuing.", state->train);
                    clockDelayBy(clock, 25);

                    state->direction = ~state->direction;
                }
                else
                {
                    logMessage("[Train %d] Continuing.", state->train);
                }
                state->isReversing = 0;
                locomotiveThrottle(state, 11);   
            }
            
            while(qBranchAction.count)
            {
                U8 action, branch;
                assertOk(queueU8Pop(&qBranchId, &branch));
                assertOk(queueU8Pop(&qBranchAction, &action));
                
                MessageEnvelope e;
                SwitchRequest req;
                req.startTime = clockTime(clock);
                req.endTime = req.startTime;
                req.indBranchNode = 2 * branch + 80;
                req.branchId = branch;
                req.direction = (SwitchState)action;

                e.type = MESSAGE_SWITCH_ALLOCATE;
                e.message.MessageArbitrary.body = (U32*)&req;

                sysSend(state->sSwitchServer.value, &e, &e);
            }
        }
    }
}

void locomotiveMakePrediction (LocomotiveState* state)
{
    U8 i = 0;
    TrackNode* ip = state->sensor->edge[DIR_AHEAD].dest;
    TrackNode* ipFail[2] = { 0, 0 };
    
    memset(state->predictDistance, 0, sizeof(U32)*4);
    memset(state->predictTime, 0, sizeof(U32)*4);
    memset(state->predictSensor, 0xFF, sizeof(U32)*4);

    while(ip)
    {
        if(ip->type == eNodeExit || ip->type == eNodeSensor)
        {
            if(ip->type == eNodeSensor)
            {
                state->predictSensor[i] = ip->num; 
            }
            i++;
            
            if( i >= 4 )
            {
                break;
            }
            else if( i < 2 )
            {
                state->predictDistance[i] = state->predictDistance[i-1];
                state->predictTime[i] = state->predictTime[i-1];
            }
            else
            {
                ip = ipFail[ i - 2 ];
                continue;
            }
        }

        if(ip->type == eNodeBranch)
        {
            MessageEnvelope env;
            env.message.MessageU32.body = (ip - (state->graph + 80)) / 2;
            env.type = MESSAGE_SWITCH_READ;
            sysSend(state->sSwitchServer.value, &env, &env);

            SwitchState sw = (SwitchState)env.message.MessageU32.body;
            U32 swv = ((sw == eCurved) ? DIR_CURVED : DIR_STRAIGHT);
            
            if(!ipFail[0]) 
            {
                U32 swf = ((sw == eCurved) ? DIR_STRAIGHT: DIR_CURVED);
                state->predictDistance[2] = state->predictDistance[i] + ip->edge[swf].dist;
                state->predictTime[2] = state->predictTime[i] + ip->edge[swf].dt;
                ipFail[0] = ip->edge[swf].dest;
            }
            else if(!ipFail[1])
            {
                U32 swf = ((sw == eCurved) ? DIR_STRAIGHT: DIR_CURVED);
                state->predictDistance[3] = state->predictDistance[i] + ip->edge[swf].dist;
                state->predictTime[3] = state->predictTime[i] + ip->edge[swf].dt;
                ipFail[1] = ip->edge[swf].dest;
            }
            
            state->predictDistance[i] += ip->edge[swv].dist;
            state->predictTime[i] += ip->edge[swv].dt;
            ip = ip->edge[swv].dest;            
        }
        else
        {
            state->predictDistance[i] += ip->edge[DIR_AHEAD].dist; 
            state->predictTime[i] += ip->edge[DIR_AHEAD].dt; 
            ip = ip->edge[DIR_AHEAD].dest;
        }
    }

    for(i = 0; i < 4; ++i)
    {
        state->predictDistance[i] += state->sensor->edge[DIR_AHEAD].dist;
        state->predictTime[i] = trainPhysicsGetTime(&state->physics, state->predictDistance[i]) / 1000 - state->predictTime[i];
    }
}

void locomotiveSensorUpdate (LocomotiveState* state, U32 sensorIndex, U32 deltaTime, U32 errorDelta)
{
    const char * strPredictTrain = "[Train %2d] At %c%2d | Predict %c%2d | T %dkt | D %dmm";
    
    U8 currentSensor = state->predictSensor[sensorIndex];
    S32 distance = state->predictDistance[sensorIndex];
    S32 time = state->predictTime[sensorIndex];
    U8 isFirstTime = (state->sensor ? 0 : 1);

    state->sensor = state->graph + currentSensor;
    locomotiveMakePrediction(state);

    logMessage( strPredictTrain, 
                state->train, 
                'A' + currentSensor / 16,
                1 + (currentSensor % 16),
                'A' + state->predictSensor[0] / 16,
                1 + (state->predictSensor[0] % 16), 
                state->predictTime[0],
                state->predictDistance[0]);


    // Reallocate track so that we remove old track.
    // check if sensor is in the list first :P
    ListU32Node* ir = state->allocatedTrack.head;
    while(ir)
    {
        if((ir->data & 0xFF) == (state->sensor->num / 2))
            break;
        ir = ir->next;
    }
    if(ir)
    {
        U32 d;
        // Yay! Reallocate til this point
        while(state->allocatedTrack.head != ir)
        {
            listU32PopFront(&state->allocatedTrack, &d);
        }
        assertOk(trainAllocateTrack(state->train, state->allocatedTrack.head));
        state->allocatedDistance = 0;
    }

    if( !state->isStopping && (state->gotoSensor == currentSensor) )
    {
        if(!state->shouldStop)
        {
            locomotiveThrottle(state, state->speed);

            do
            {
                do
                {
                    nextRandU32(&state->random);
                    state->gotoSensor = state->random % 80;
                } while (state->gotoSensor == 0x01 || state->gotoSensor == 0x05 || state->gotoSensor == 0x06 ||
                         state->gotoSensor == 0x08 || state->gotoSensor == 0x0C || state->gotoSensor == 0x0D ||
                         state->gotoSensor == 0x0E || state->gotoSensor == 0x17 || state->gotoSensor == 0x1B ||
                         state->gotoSensor == 0x19 || state->gotoSensor == 0x22 || state->gotoSensor == 0x27);
            } while( pathFind(state->graph, state->sensor->num, state->gotoSensor, &state->destinationPath) < 0 );
        }
    }
    else if( state->gotoSensor < 0xFF )
    {
        pathFind(state->graph, state->sensor->num, state->gotoSensor, &state->destinationPath);
    }

    if(!isFirstTime)
    {
        S32 traveledDistance = trainPhysicsGetDistance(&state->physics);
        
        trainPhysicsStep(&state->physics, deltaTime);
        
        S32 deltaX = distance - traveledDistance;
        S32 deltaT = time - errorDelta/1000;
        S32 accelReport = trainPhysicsReport(&state->physics, distance, errorDelta, deltaT);
        
        locomotiveStep(state, 0);
        
        TrackEdge* edgeFrom = state->sensor->reverse->edge->reverse;

        if (accelReport != 1)
        {
            S32 trueDelta = deltaT;

            if (((deltaT < 120 && deltaT > 10) || (deltaT > -120 && deltaT < -10)))
            {
                if (edgeFrom->dt == 0)
                {
                    edgeFrom->dt = trueDelta/2;
                }
                else
                {
                    edgeFrom->dt = (edgeFrom->dt * 1900 + trueDelta * 100)/2000;
                }
            }

            if (deltaX < 50 && deltaX > -50)
            {
                if (edgeFrom->dx == 0)
                {
                    edgeFrom->dx = deltaX;
                }
                else
                {
                    edgeFrom->dx = (edgeFrom->dx * 900 + deltaX * 100)/1000;
                }
            }
        }
    }

    if (state->gotoSpeed < 14)
    {
        locomotiveThrottle(state, state->gotoSpeed);
        state->gotoSpeed = 0xFF;
    }

}


