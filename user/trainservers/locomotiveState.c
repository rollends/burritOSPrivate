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

    memset(state->predictSensor, 0xFF, sizeof(U32) * 4);
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
            // Still stopping. DONT DO ANYTHING ELSE.
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
    U32 clearance = (3 * state->stopDistance) / 2 + 230;
    state->distanceRequired += clearance;

    if (state->hasConflict == 1)
    {
        // Try a reallocation.
        // we already stopped, wait a random amount (doesn't matter if physics tick is delayed. calculations are 
        // not being done for the 0 speed)
        clockDelayBy(nsWhoIs(Clock), 100 + 20 * (nextRandU32(&state->random) % 10 + 1));
        state->hasConflict = 2;
    }
    else if (state->gotoSensor < 0xFF)
    {
        TrackNode* ip = state->sensor;   
        TrackRequest requests[50];
        U8 iRequest = 0;

        ListU32Node* node = state->destinationPath.path.head;
        assert(node != 0);
        U8 indNext = node->data;
        
        U8 aBranchAction[8], aBranchId[8];
        U16 aBranchDelay[8];
        QueueU8 qBranchAction, qBranchId;
        QueueU16 qBranchDelay;
        queueU8Init(&qBranchAction, aBranchAction, 8);
        queueU8Init(&qBranchId, aBranchId, 8);
        queueU16Init(&qBranchDelay, aBranchDelay, 8);

        U32 distToTravel = 0;
        U32 sensorCount = 0;
        TrackNode* nextSensor = 0;
        U32 distToStop = 0;
        
        do
        {
            assert(iRequest < 50);
            assert(state->distanceRequired < 5000);

            if( indNext != 0xFF )
            {
                node = node->next;
                if( node )
                    indNext = node->data;
                else
                    indNext = 0xFF;
            }

            U32 indCurrent = ip - state->graph;
            TrackEdge* edge = ip->edge + DIR_AHEAD;
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
                    
                    if( swn != sw && distToTravel >= state->physics.distance)
                    {
                        S32 kticks = trainPhysicsGetTime(&state->physics, distToTravel - state->physics.distance) / 1000;
                        if( swn != sw && (kticks >= 400) && ((distToTravel - state->physics.distance) >= 200) )
                        {
                            if((state->distanceRequired > 0) || (sensorCount < 2))
                            {
                                assertOk(queueU8Push(&qBranchId, ip->num));
                                assertOk(queueU8Push(&qBranchAction, swn));
                            }
                            sw = swn;
                        }
                    }
                }
                edge = &ip->edge[(sw == eCurved ? DIR_CURVED : DIR_AHEAD)];  
            }
            else if(ip->type == eNodeSensor)
            {
                if((state->distanceRequired > 0) || (sensorCount < 2))
                {
                    if(!nextSensor)
                    {
                        nextSensor = ip;
                    }
                    sensorCount++;
                }
            }

            if((state->distanceRequired > 0) || (sensorCount <= 2))
            {
                requests[iRequest].trainId = state->train;
                requests[iRequest].indNode = indCurrent / 2;
                requests[iRequest].pReverseRequest = 0;
                requests[iRequest].pForwardRequest = &requests[iRequest+1];
                iRequest++;
                state->distanceRequired -= (edge->dist - edge->dx);
                distToTravel += (edge->dist - edge->dx);
            }

            if( state->shouldStop && ip->num == state->gotoSensor )
                break;

            distToStop += (edge->dist);

            if( ip->type == eNodeExit ) 
                break;
         
            ip = edge->dest;
        } while(state->shouldStop || (state->distanceRequired > 0) || (sensorCount < 2) );
        distToStop -= state->physics.distance;

        // End off shitty linked list..
        requests[iRequest-1].pForwardRequest = 0;
       
        S32 failedNode = trainAllocateTrack(state->train, requests);

        if(state->shouldStop && state->stopDistance >= distToStop)
        {
            state->isStopping = 1;
            state->gotoSensor = 0xFF;
            locomotiveThrottle(state, 0);
        }
        else if(state->distanceRequired > 0 && ip->type == eNodeExit)
        {
            // STOP!
            state->isStopping = 1;
            locomotiveThrottle(state, 0);
        }
        else if(failedNode < 0)
        {
            //printf("\033[s\033[%d;1H[Train %d] Failed Allocation at %d.\033[u", 
             //   (train == 64 ? 51 : 52), 
             //   train, 
             //   2*(U32)(-failedNode));
            
            state->hasConflict = 1;
            
            // Failed allocation
            if (state->speed == 0)
            {
                state->isReversing = ~state->isReversing;
                state->sensor = state->sensor->reverse;
                //state->sensor = (state->graph + state->predictSensor[0])->reverse;
                //if(state->physics.distance <= state->predictDistance[0])
                //    state->physics.distance = state->predictDistance[0] - state->physics.distance;
                //else
                    state->physics.distance = 0;
                
                pathFind(state->graph, state->sensor->num, state->gotoSensor, &state->destinationPath);
                locomotiveMakePrediction(state);
            }
            else
            {
                char buffer[512];
                sprintf(buffer, "[Train %d] Yielding!", state->train);
                logMessage(buffer);
                
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

                    char buffer[512];
                    sprintf(buffer, "[Train %d] Reversing.", state->train);
                    logMessage(buffer);

                    clockDelayBy(clock, 25);
                }
                else
                {
                    char buffer[512];
                    sprintf(buffer, "[Train %d] Continuing.", state->train);
                    logMessage(buffer);
                }
                state->isReversing = 0;
                locomotiveThrottle(state, 11);   
            }
            
            while(qBranchAction.count)
            {
                U8 action, branch;
                U16 delay = 0;
                assertOk(queueU8Pop(&qBranchId, &branch));
                assertOk(queueU8Pop(&qBranchAction, &action));
                
                MessageEnvelope e;
                SwitchRequest req;
                req.startTime = clockTime(clock) + delay;
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

    while(ip && ip->type != eNodeExit)
    {
        if(ip->type == eNodeSensor)
        {
            state->predictSensor[i++] = ip->num; 

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
    U8 currentSensor = state->predictSensor[sensorIndex];
    S32 distance = state->predictDistance[sensorIndex];
    S32 time = state->predictTime[sensorIndex];
    U8 isFirstTime = (state->sensor ? 1 : 0);

    state->sensor = state->graph + currentSensor;
    locomotiveMakePrediction(state);

    if( !state->isStopping && (state->gotoSensor == currentSensor) )
    {
        if(!state->shouldStop)
        {
            locomotiveThrottle(state, 11);

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

    if(isFirstTime)
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


