#include "common/common.h"
#include "kernel/kernel.h"
#include "hardware/hardware.h"
#include "trains/trains.h"

#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"

typedef struct
{
    U32 triggeredSensor;
    U32 sensorSkip;
    U32 distance;
    U32 predictionDistance[4];
    U32 prediction[4];
    S32 time[4];
} GPSUpdate;

static void LocomotiveRadio(void);
static void LocomotiveGPS(void);
static void PositionUpdateCourier(void);

static void PhysicsTick(void)
{
    MessageEnvelope env;
    TaskID clock = nsWhoIs(Clock);
    
    for(;;)
    {
        clockDelayBy(clock, 3);
        sysSend(sysPid(), &env, &env); 
    }
}

static TrackNode* getLocomotiveTrackGraph(U8 trainId)
{
    TaskID loco;

    if( trainWhoIs(trainId, &loco) < 0 )
        return 0;

    MessageEnvelope env;
    env.type = MESSAGE_RPS; // Random number lawl
    sysSend(loco.value, &env, &env);

    return (TrackNode*)env.message.MessageArbitrary.body;
}


void Locomotive(void)
{
    const char * strPredictOldTrain = "\033[s\033[31;7m\033[s\033[44;1H\033[2KLast Prediction:\tTrain %2d | Location %c%2d | Time %dkt\033[u\033[m\033[u";
    const char * strPredictClearTrain = "\033[s\033[44;1H\033[2K\033[u";
    const char * strPredictTrain = "\033[s\033[48;1H\033[2KNext Prediction:\tTrain %2d | Location %c%2d | Time %dkt\033[u";
    const char * strFoundTrain =   "\033[s\033[45;1H\033[2KRecorded Data:\t\tTrain %2d | Location %c%2d | Time %dkt | \033[1mDeltaT %dkt\033[m | Correction: %dkt\r\n\t\t\tReal Distance: %dmm | Physics Distance: %dmm | DeltaX: %dmm                \033[u";

    TrackNode graph[TRACK_MAX]; 
    
    TaskID parent = VAL_TO_ID( sysPid() );
    TaskID from;
    MessageEnvelope env;

    // Find out the ID of the train I'm driving around.
    sysReceive(&from.value, &env);
    assert(from.value == parent.value);
    sysReply(from.value, &env);
    U8 train = env.message.MessageU8.body;
   
    // Register ourselves.
    trainRegister(train);

    // Setup speed 
    TaskID sTrainDriver = nsWhoIs(Train);
    TaskID sSwitchOffice = nsWhoIs(TrainSwitchOffice);
    TaskID sClock = nsWhoIs(Clock);
    trainSetSpeed(sTrainDriver, train, 0);

    // Initialize Physics
    TrainPhysics physics;
    trainInit(&physics, train);
    trainPhysicsSetSpeed(&physics, 0);
    
    // Initialize our track graph
    init_tracka(graph);
 
    // Startup Radio comm (for commands)
    assert(sysPriority() < 31);

    // Start Train 'GPS' and physics tick
    TaskID tPosGPS = VAL_TO_ID(sysCreate(sysPriority() + 1, &PositionUpdateCourier));
    TaskID tPhysicsTick = VAL_TO_ID(sysCreate(sysPriority() - 1, &PhysicsTick));

    sysSend(tPosGPS.value, &env, &env);

    TimerState tickTimer;
    TimerState errorTimer;
    timerStart(TIMER_4, &tickTimer);
    timerStart(TIMER_4, &errorTimer);

    TrackNode* previousSensor = 0;
    U8 reverseCourierState = 0;
    TaskID reverseCourier = { 0 };

    S16     throttle = 0;
    char    nextSensorGroup = 0;
    U8      nextSensorId = 0;
    U32     predictTime[4] = { 0 };
    U32     gotoSensor = 0xFFFF;
    //S32     stopDistance = 0;
    U32     stopping = 0;
    U8      setSpeed = 0xFF;
    U8      isLaunching = 1;
    U8      hasConflict = 0;
    U8      shouldStop = 0;
    U32     random = (train + 1) ^ (0x1a3fb13f);
    GPSUpdate previousUpdate;
    GPSUpdate update;

    GraphPath destinationPath;
    S32     previousStopDistance = 0;
    U32     distancePastPreviousSensor = 0;

    trainSetSpeed(sTrainDriver, train, 5);
    for(;;)
    {
        sysReceive(&from.value, &env);

        if( from.value == tPhysicsTick.value )
        {
            sysReply(from.value, &env);
            timerSample(TIMER_4, &tickTimer);
            S32 delta = trainPhysicsStep(&physics, tickTimer.delta);
            
            distancePastPreviousSensor += delta;
            S32 requiredDistance = distancePastPreviousSensor;

            if(!physics.targetSpeed)
            {
                requiredDistance += ((previousStopDistance) + 300);
            }
            else
            {
                previousStopDistance = trainPhysicsStopDist(&physics);
                requiredDistance += ((previousStopDistance) + 300);
            }
            
            if(stopping)
            {
                stopping = (physics.speed != 0);
            }

            if (hasConflict)
            {
                if(!stopping)
                {
                    // Try a reallocation.
                    // we already stopped, wait a random amount (doesn't matter if physics tick is delayed. calculations are 
                    // not being done for the 0 speed)
                    clockDelayBy(sClock, 100 + 50*(nextRandU32(&random) % 10));
                    hasConflict = 0;
                }
            }
            else if (gotoSensor < 0xFFFF)
            {
                TrackNode* ip = previousSensor;   
                TrackRequest requests[50];
                U8 iRequest = 0;

                ListU32Node* node = destinationPath.path.head;
                U8 indNext = node->data;
                
                U8 aBranchAction[4], aBranchId[4];
                QueueU8 qBranchAction, qBranchId;
                queueU8Init(&qBranchAction, aBranchAction, 4);
                queueU8Init(&qBranchId, aBranchId, 4);
                U32 distToTravel = 0;
                U32 sensorCount = 0;
                U32 distToFinalSensor = 0;
                TrackNode* nextSensor = 0;
                U32 distToNextSensor = 0;
                U32 distToStop = 0;
                do
                {
                    assert(iRequest < 50);
                    if( indNext != 0xFF )
                    {
                        node = node->next;
                        if( node )
                            indNext = node->data;
                        else
                            indNext = 0xFF;
                    }

                    U32 indCurrent = ip - graph;
                    TrackEdge* edge = ip->edge + DIR_AHEAD;
                    if(ip->type == eNodeBranch)
                    {
                        SwitchState sw = eStraight;
                        
                        env.type = MESSAGE_SWITCH_READ;
                        env.message.MessageU32.body = (indCurrent - 80) / 2;
                        sysSend(sSwitchOffice.value, &env, &env);
                        sw = (SwitchState)env.message.MessageU32.body;

                        if( indNext != 0xFF )
                        {
                            SwitchState swn = ((ip->edge[DIR_AHEAD].dest - graph) == indNext 
                                ? eStraight 
                                : eCurved);

                            U32 kticks = trainPhysicsGetTime(&physics, distToTravel - physics.distance) / 1000;
                            if( swn != sw && (kticks >= 250) )
                            {
                                if((requiredDistance > 0) || (sensorCount < 2))
                                {
                                    queueU8Push(&qBranchId, ip->num);
                                    queueU8Push(&qBranchAction, swn);
                                }
                                sw = swn;
                            }
                        }
                        edge = &ip->edge[(sw == eCurved ? DIR_CURVED : DIR_AHEAD)];  
                    }
                    else if(ip->type == eNodeSensor)
                    {
                        if((requiredDistance > 0) || (sensorCount < 2))
                        {
                            if(!nextSensor)
                            {
                                nextSensor = ip;
                                distToNextSensor = distToTravel;
                            }
                            else if(sensorCount < 2)
                            {
                                distToFinalSensor = distToTravel;
                            }
                            sensorCount++;
                        
                        
                            requests[iRequest].trainId = train;
                            requests[iRequest].indNode = indCurrent / 2;
                            requests[iRequest].pReverseRequest = 0;
                            requests[iRequest].pForwardRequest = &requests[iRequest+1];
                            iRequest++;
                            requiredDistance -= (edge->dist - edge->dx);
                            distToTravel += (edge->dist - edge->dx);
                        }
                    }
 
                    if( shouldStop && ip->num == gotoSensor )
                        break;

                    distToStop += (edge->dist - edge->dx);

                    if( ip->type == eNodeExit ) 
                        break;
                 
                    ip = edge->dest;
                } while(shouldStop || (requiredDistance > 0) || (sensorCount < 2) );
                distToStop -= physics.distance;
                printf("\033[s\033[44;1H\033[2KDist to Stop: %d\033[u", distToStop);

                // End off shitty linked list..
                requests[iRequest-1].pForwardRequest = 0;
               
                S32 failedNode = trainAllocateTrack(train, requests);

                if(shouldStop && trainPhysicsStopDist(&physics) >= distToStop)
                {
                    stopping = 1;
                    throttle = 0;
                    gotoSensor = 0xFFFF;
                    trainSetSpeed(sTrainDriver, train, throttle);
                    trainPhysicsSetSpeed(&physics, throttle);
                }
                else if(stopping)
                {
                    // do nothing.
                }
                else if(distancePastPreviousSensor > distToFinalSensor)
                {
                    distancePastPreviousSensor = 0;
                }
                else if(requiredDistance > 0 && ip->type == eNodeExit)
                {
                    // STOP!
                    stopping = 1;
                    throttle = 0;
                    trainSetSpeed(sTrainDriver, train, throttle);
                    trainPhysicsSetSpeed(&physics, throttle);
                }
                else if(failedNode < 0)
                {
                    
                    printf("\033[s\033[%d;1H[Train %d] Failed Allocation at %d.\033[u", 
                        (train == 64 ? 51 : 52), 
                        train, 
                        2*(U32)(-failedNode));
                    
                    hasConflict = 1;
                    
                    // Failed allocation
                    if (throttle == 0)
                    {
                        if(reverseCourierState == 1)
                        {
                            // Courier is waiting for us to send command.
                            sysReply(reverseCourier.value, &env);
                            reverseCourierState = 0; 
                            // Now wait for courier to come back around.
                        }
                        else if(reverseCourierState == 0)
                        {
                            // Courier has missed message.
                            reverseCourierState = 2;
                            // Make sure when courier comes around we send it the reverse command.
                        }
                        distancePastPreviousSensor = distToNextSensor - distancePastPreviousSensor;
                        //previousSensor = nextSensor->reverse;
                        distancePastPreviousSensor = 0;
                        previousSensor = previousSensor->reverse;

                        pathFind(graph, previousSensor->num, gotoSensor, &destinationPath);

                        env.type = MESSAGE_TRAIN_REVERSE;
                        env.message.MessageU16.body = (train << 8) | 0x0F;
                        sysSend(sTrainDriver.value, &env, &env);
                    }
                    else
                    {
                        stopping = 1;
                        throttle = 0;
                        trainSetSpeed(sTrainDriver, train, throttle);
                        trainPhysicsSetSpeed(&physics, throttle);
                    }
                }
                else
                {    
                    while(qBranchAction.count)
                    {
                        U8 action, branch;
                        queueU8Pop(&qBranchId, &branch);
                        queueU8Pop(&qBranchAction, &action);
                        
                        MessageEnvelope e;
                        SwitchRequest req;
                        req.startTime = clockTime(nsWhoIs(Clock));
                        req.endTime = req.startTime;
                        req.indBranchNode = 2 * branch + 80;
                        req.branchId = branch;
                        req.direction = (SwitchState)action;

                        e.type = MESSAGE_SWITCH_ALLOCATE;
                        e.message.MessageArbitrary.body = (U32*)&req;

                        sysSend(sSwitchOffice.value, &e, &e);
                    }                
                }
            }
        }
        else if( from.value == tPosGPS.value )
        {
            previousUpdate = update; 
            update = *(GPSUpdate*)(env.message.MessageArbitrary.body);
            sysReply(tPosGPS.value, &env);
            U8      currentSensor = update.triggeredSensor;
            char    sensorGroup = 'A' + currentSensor / 16;
            U8      sensorId = currentSensor % 16 + 1;
 
            timerSample(TIMER_4, &errorTimer);
            
            if( isLaunching )
            {
                isLaunching = 0;
                trainSetSpeed(sTrainDriver, train, 0);
                
                TaskID tRadio = VAL_TO_ID(sysCreate(sysPriority()+1, &LocomotiveRadio));
                env.message.MessageU8.body = train;
                sysSend(tRadio.value, &env, &env);
            }
            
            if( previousSensor != 0)
            {
                timerSample(TIMER_4, &tickTimer);
                trainPhysicsStep(&physics, tickTimer.delta);

                nextSensorGroup = previousUpdate.prediction[update.sensorSkip] / 16 + 'A';
                nextSensorId = previousUpdate.prediction[update.sensorSkip] % 16 + 1;
                
                S32 deltaT = predictTime[update.sensorSkip] - errorTimer.delta/1000;
                if (deltaT < -50 || deltaT > 50)
                {
                    printf(strPredictOldTrain, train, nextSensorGroup, nextSensorId, predictTime[update.sensorSkip]);
                }
                else
                {
                    printf(strPredictClearTrain);
                }

                if( !stopping && (gotoSensor == currentSensor) )
                {
                    if(shouldStop)
                    {
                    }
                    else
                    {
                        throttle = 13;
                        trainSetSpeed(sTrainDriver, train, throttle);
                        trainPhysicsSetSpeed(&physics, throttle);

                        do
                        {
                            do
                            {
                                nextRandU32(&random);
                                gotoSensor = random % 80;
                            } while (gotoSensor == 0x01 || gotoSensor == 0x05 || gotoSensor == 0x06 ||
                                     gotoSensor == 0x08 || gotoSensor == 0x0B || gotoSensor == 0x0D ||
                                     gotoSensor == 0x0E || gotoSensor == 0x17 || gotoSensor == 0x1B ||
                                     gotoSensor == 0x19 || gotoSensor == 0x22);
                        } while( pathFind(graph, currentSensor, gotoSensor, &destinationPath) < 0 );
                    }
                }
                else if( gotoSensor < 0xFFFF )
                {
                    pathFind(graph, currentSensor, gotoSensor, &destinationPath);
                }

                // Predict
                S32 distance = update.distance;
                S32 traveledDistance = trainPhysicsGetDistance(&physics);
                S32 deltaX = distance - traveledDistance;
                TrackEdge* edgeFrom = (graph + currentSensor)->reverse->edge->reverse;
                
                assert(distance < 5000);

                S32 accelReport = trainPhysicsReport(&physics, distance, errorTimer.delta, deltaT);
                
                if (setSpeed < 14)
                {
                    throttle = setSpeed;
                    trainPhysicsSetSpeed(&physics, setSpeed);
                    trainSetSpeed(sTrainDriver, train, setSpeed);
                    setSpeed = 0xFF;
                }

                if (accelReport != 1)
                {
                    S32 trueDelta = deltaT + previousUpdate.time[update.sensorSkip];

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

                nextSensorGroup = update.prediction[0] / 16 + 'A';
                nextSensorId = update.prediction[0] % 16 + 1;

                U8 i = 0;
                for(i = 0; i < 4; ++i)
                {
                    predictTime[i] = trainPhysicsGetTime(&physics, update.predictionDistance[i]) / 1000 - update.time[i];
                }

                printf(strPredictTrain, train, nextSensorGroup,
                       nextSensorId, predictTime[0]);

                printf(strFoundTrain, train, sensorGroup, sensorId,
                       errorTimer.delta/1000, deltaT,
                       previousUpdate.time[update.sensorSkip],
                       distance, traveledDistance, deltaX,
                       physics.velocity);
            }

            previousSensor = graph + currentSensor;
            distancePastPreviousSensor = 0;
        }
        else
        {
            switch(env.type)
            {
            case MESSAGE_RPS:
                env.message.MessageArbitrary.body = (U32*)graph;
                sysReply(from.value, &env);
                break;

            case MESSAGE_TRAIN_GOTO:
            {
                sysReply(from.value, &env);
                gotoSensor = env.message.MessageU32.body;
                shouldStop = 0;
                assert( pathFind(graph, previousSensor->num, gotoSensor, &destinationPath) >= 0 );
                
                throttle = 13;
                trainSetSpeed(sTrainDriver, train, throttle);
                trainPhysicsSetSpeed(&physics, throttle);

                break;
            }

            case MESSAGE_TRAIN_STOP:
            {
                sysReply(from.value, &env);
                gotoSensor = env.message.MessageU32.body;
                shouldStop = 1;
                assert( pathFind(graph, previousSensor->num, gotoSensor, &destinationPath) >= 0 );
                
                throttle = 13;
                trainSetSpeed(sTrainDriver, train, throttle);
                trainPhysicsSetSpeed(&physics, throttle);
                break;
            }

            case MESSAGE_COURIER:
                reverseCourier = from;
                if( reverseCourierState == 0 )
                {
                    reverseCourierState = 1; // Waiting for a reverse command
                }
                else if(reverseCourierState == 2)
                {
                    env.type = MESSAGE_TRAIN_REVERSE;
                    sysReply(reverseCourier.value, &env);
                    reverseCourierState = 0;
                }
                break;

            case MESSAGE_TRAIN_SET_SPEED:
                throttle = env.message.MessageU8.body;
                if (physics.velocity == 0)
                {
                    trainPhysicsSetSpeed(&physics, throttle);
                    trainSetSpeed(sTrainDriver, train, throttle);
                    setSpeed = 0xFF;
                }
                else
                {
                    setSpeed = throttle;
                }

                if (throttle != 0)
                {
                    if (stopping == 1)
                    {
                        previousSensor = 0;
                    }
                    //stopping = 0;
                    //stopDistance = 0;
                }
                sysReply(from.value, &env);
                break;

            case MESSAGE_TRAIN_REVERSE:
                if(reverseCourierState == 1)
                {
                    // Courier is waiting for us to send command.
                    sysReply(reverseCourier.value, &env);
                    reverseCourierState = 0; 
                    // Now wait for courier to come back around.
                }
                else if(reverseCourierState == 0)
                {
                    // Courier has missed message.
                    reverseCourierState = 2;
                    // Make sure when courier comes around we send it the reverse command.
                }
                previousSensor = previousSensor->reverse;
 
                trainSetSpeed(sTrainDriver, train, 0);
                clockDelayBy(nsWhoIs(Clock), 20 * (throttle) + 100);

                env.message.MessageU16.body = (train << 8) | 0x0F;
                sysSend(sTrainDriver.value, &env, &env);

                clockDelayBy(nsWhoIs(Clock), 5);
                trainSetSpeed(sTrainDriver, train, throttle);
                sysReply(from.value, &env);
                break;
            case MESSAGE_TRAIN_DUMP_ACCEL:
            {
                sysReply(from.value, &env);
                U8 dump = env.message.MessageU8.body;
                U8 i;
                for (i = 0; i < 14; i++)
                {
                    printf("\033[s\033[%d;30H \033[2K %d \033[u", i + 9, physics.accelMap[dump][i]);
                }
                break;
            }
            case MESSAGE_TRAIN_DUMP_VEL:
            {
                sysReply(from.value, &env);
                U8 i;
                for (i = 0; i < 14; i++)
                {
                    printf("\033[s\033[%d;45H \033[2K %d \033[u", i + 9, physics.speedMap[i]);
                }
                break;
            }
            }
        }
    }
}

static void LocomotiveRadio(void)
{
    TaskID loco; 
    
    MessageEnvelope env;
    sysReceive(&loco.value, &env);
    sysReply(loco.value, &env);

    U8 train = env.message.MessageU8.body;
    printf("\033[s\033[43;1HTrain %d\033[u", train);
    for(;;)
    {
        env.message.MessageU8.body = train;
        pollTrainCommand(train, &env);
        sysSend(loco.value, &env, &env);
    }
}

static void LocomotiveSensor(void)
{
    TaskID sSensors = nsWhoIs(TrainSensors);
    TaskID tParent = VAL_TO_ID(sysPid());
    U32 sensors[5];
    MessageEnvelope env;
    for(;;)
    {
        trainReadAllSensors(sSensors, sensors);
        env.message.MessageArbitrary.body = sensors;
        sysSend(tParent.value, &env, &env);
    }
}
static void ReverseUpdateCourier(void)
{
    // Parent is Courier..we get mesage to hook up
    TaskID id;
    MessageEnvelope env;

    sysReceive(&id.value, &env);
    sysReply(id.value, &env);

    TaskID gps = VAL_TO_ID(env.message.MessageU32.body >> 16);
    TaskID loco = VAL_TO_ID(env.message.MessageU32.body & 0xFFFF);

    for(;;)
    {
        env.type = MESSAGE_COURIER;
        sysSend(loco.value, &env, &env);
        sysSend(gps.value, &env, &env);
    }
}
static void PositionUpdateCourier(void)
{
    assert(sysPriority() < 31);
    TaskID gps = VAL_TO_ID(sysCreate(sysPriority() + 1, &LocomotiveGPS));
    TaskID parent = VAL_TO_ID(sysPid());
    MessageEnvelope env;
    
    sysReceive(&parent.value, &env);
    sysReply(parent.value, &env);
    //U8 train = env.message.MessageU8.body;
    sysSend(gps.value, &env, &env);

    env.message.MessageU32.body = (gps.value << 16) | parent.value;
    sysSend(sysCreate(sysPriority(), &ReverseUpdateCourier), &env, &env);

    for(;;)
    {
        sysSend(gps.value, &env, &env);
        sysSend(parent.value, &env, &env);
    }
}



static void findNextSensors(TrackNode* graph, TrackNode* current, U32* nextSensors, U32* dist, U32* time)
{
    // FORWARD PREDICTION 
    TaskID sSwitchOffice = nsWhoIs(TrainSwitchOffice);
    TrackNode* ip = current;
    U8 i = 0;
    
    TrackNode* ipFail[2] = { 0, 0 };
    memset(dist, 0, sizeof(U32)*4);
    memset(time, 0, sizeof(U32)*4);
    memset(nextSensors, 0xFF, sizeof(U32)*4);

    while(ip && ip->type != eNodeExit)
    {
        if(ip->type == eNodeSensor)
        {
            nextSensors[i++] = ip->num; 

            if( i >= 4 ) 
            {
                break;
            }
            else if( i < 2 )
            {
                dist[i] = dist[i-1];
                time[i] = time[i-1];
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
            env.message.MessageU32.body = (ip - (graph + 80)) / 2;
            env.type = MESSAGE_SWITCH_READ;
            sysSend(sSwitchOffice.value, &env, &env);

            SwitchState sw = (SwitchState)env.message.MessageU32.body;
            U32 swv = ((sw == eCurved) ? DIR_CURVED : DIR_STRAIGHT);
            
            if(!ipFail[0]) 
            {
                U32 swf = ((sw == eCurved) ? DIR_STRAIGHT: DIR_CURVED);
                dist[2] = dist[i] + ip->edge[swf].dist;
                time[2] = time[i] + ip->edge[swf].dt;
                ipFail[0] = ip->edge[swf].dest;
            }
            else if(!ipFail[1])
            {
                U32 swf = ((sw == eCurved) ? DIR_STRAIGHT: DIR_CURVED);
                dist[3] = dist[i] + ip->edge[swf].dist;
                time[3] = time[i] + ip->edge[swf].dt;
                ipFail[1] = ip->edge[swf].dest;

            }
            
            dist[i] += ip->edge[swv].dist;
            time[i] += ip->edge[swv].dt;
            ip = ip->edge[swv].dest;            
        }
        else
        {
            dist[i] += ip->edge[DIR_AHEAD].dist; 
            time[i] += ip->edge[DIR_AHEAD].dt; 
            ip = ip->edge[DIR_AHEAD].dest;
        }
    }
}
static void LocomotiveGPS(void)
{
    U8 train = 0;
    
    {
        TaskID temp;
        MessageEnvelope env;
        sysReceive(&temp.value, &env);
        sysReply(temp.value, &env);
        train = env.message.MessageU8.body;
    }

    TrackNode* graph = getLocomotiveTrackGraph(train);
    TrackNode* position = 0;

    U32 sensors[5], nextSensors[4], distances[4], time[4];
    U32 i = 0;
 
    assert(sysPriority() < 31);
    TaskID tSensors = VAL_TO_ID(sysCreate(sysPriority() + 1, &LocomotiveSensor));
    TaskID tPosCourier = VAL_TO_ID(sysPid());
    U8 courierWaiting = 0;

    GPSUpdate update;
    for(;;)
    {
        TaskID from;
        MessageEnvelope env;
        sysReceive(&from.value, &env);

        if( from.value == tSensors.value )
        {
            memcpy(sensors, env.message.MessageArbitrary.body, 5);
            sysReply(tSensors.value, &env);
            
            U32 sensorHit = 0xFF;
            
            // There was a previous position
            for(i = 0; (sensorHit != 1) && (i < 5); ++i)
            {
                U8 c = 0;
                for(c = 0; (sensorHit != 1) && (c < 16); ++c)
                {
                    U16 isHit = (sensors[i] & (1 << (15-c)));
                    
                    if( !isHit ) 
                        continue;
                    
                    U8 v = i * 16 + c;
                    U8 owner = trainWhoOwnsTrack(v);
                    
                    if( owner != train && (owner != 0))
                        continue;

                    //if( owner == 0 && position != 0 )
                    //    continue;

                    if( !position )
                    {
                        sensorHit = 1;
                        nextSensors[0] = v;
                        distances[0] = 0;
                    }
                    else
                    {
                        U8 s = 0;
                        for(s = 1; s <= 4; ++s)
                        {
                            if(nextSensors[s - 1] == v)
                            {
                                sensorHit = min(sensorHit, s);
                                break;
                            }
                        }
                    }
                }
            }
            // Its what we predicted!
            if(sensorHit <= 4 && sensorHit >= 1)
            {
                position = graph + nextSensors[sensorHit-1];
                
                env.message.MessageArbitrary.body = (U32*)(&update);
                update.triggeredSensor = nextSensors[sensorHit-1];
                update.sensorSkip = sensorHit-1;
                update.distance = distances[sensorHit-1];

                findNextSensors(graph, position->edge[0].dest, nextSensors, distances, time);
                for(i = 0; i < 4; ++i)
                {
                    distances[i] += position->edge[0].dist;
                    time[i] += position->edge[0].dt;
                    
                    update.predictionDistance[i] = distances[i];
                    update.prediction[i] = nextSensors[i];
                    update.time[i] = time[i];
                }
                
                if(courierWaiting)
                {
                    courierWaiting = 0;
                    sysReply(tPosCourier.value, &env);
                }
            }
        }
        else if( from.value == tPosCourier.value )
        {
            courierWaiting = 1;
        }
        else
        {
            switch(env.type)
            {
            case MESSAGE_TRAIN_REVERSE:
            {
                position = position->reverse;
                findNextSensors(graph, position, nextSensors, distances, time);
                sysReply(from.value, &env);
                break;
            }
            }
        }
    }
}
