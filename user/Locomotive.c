#include "common/common.h"
#include "kernel/kernel.h"
#include "hardware/hardware.h"

#include "user/services/services.h"
#include "user/messageTypes.h"

#include "user/trains/trains.h"
#include "user/trains/TrainYard.h"
#include "user/trains/TrainCommander.h"

#include "trains/trainInit.h"
#include "trains/trainPhysics.h"

typedef struct
{
    U32 triggeredSensor;
    U32 sensorSkip;
    U32 distance;
    U32 predictionDistance[3];
    U32 prediction[3];
    S32 time[3];
} GPSUpdate;

static void LocomotiveRadio(void);
static void LocomotiveGPS(void);
static void PositionUpdateCourier(void);

static void RandomSpeed(void)
{
    MessageEnvelope env;
    
    U32 random = 19;
    U32 zero = 0;
    for(;;)
    {
        nextRandU32(&random);
        U8 delay = random % 40 + 30;

        nextRandU32(&random);
        U8 speed = random % 9 + 5;
        if (zero == 0)
        {
//            speed = 0;
    //        zero = 1;
        }
        else
        {
  //          zero = 0;
        }

        clockLongDelayBy(nsWhoIs(Clock), delay);

        env.message.MessageU8.body = speed;
        sysSend(sysPid(), &env, &env); 
    }
}

static void PhysicsTick(void)
{
    MessageEnvelope env;
    
    for(;;)
    {
        clockDelayBy(nsWhoIs(Clock), 3);
        sysSend(sysPid(), &env, &env); 
    }
}

static TrackNode graph[TRACK_MAX]; 

void Locomotive(void)
{
    //const char * strPredictOldTrain = "\033[s\033[31;7m\033[s\033[44;1H\033[2KLast Prediction:\tTrain %2d | Location %c%2d | Time %dkt\033[u\033[m\033[u";
    //const char * strPredictClearTrain = "\033[s\033[44;1H\033[2K\033[u";
    //const char * strPredictTrain = "\033[s\033[48;1H\033[2KNext Prediction:\tTrain %2d | Location %c%2d | Time %dkt\033[u";
    //const char * strFoundTrain =   "\033[s\033[45;1H\033[2KRecorded Data:\t\tTrain %2d | Location %c%2d | Time %dkt | \033[1mDeltaT %dkt\033[m | Correction: %dkt\r\n\t\t\tReal Distance: %dmm | Physics Distance: %dmm | DeltaX: %dmm                \033[u";
    //const char * strPhysicsTrain = "\033[s\033[50;1H\033[2KPhysics:\t\tTrain %2d | Target Speed %d | Velocity %dmm/mt | Acceleration %dmm/mt^2\033[u";

    TaskID parent = VAL_TO_ID( sysPid() );
    TaskID from;
    MessageEnvelope env;

    // Find out the ID of the train I'm driving around.
    sysReceive(&from.value, &env);
    assert(from.value == parent.value);
    sysReply(from.value, &env);
    U8 train = env.message.MessageU8.body;
   
    // Setup speed 
    TaskID sTrainDriver = nsWhoIs(Train);
    TaskID sSwitchOffice = nsWhoIs(TrainSwitchOffice);
    trainSetSpeed(sTrainDriver, train, 0);

    // Initialize Physics
    TrainPhysics physics;
    trainInit(&physics, train);
    trainPhysicsSetSpeed(&physics, 0);
    
    // Initialize our track graph
    init_tracka(graph);
 
    // Startup Radio comm (for commands)
    assert(sysPriority() < 31);
    sysSend(sysCreate(sysPriority()+1, &LocomotiveRadio), &env, &env);

    // Register ourselves.
    trainRegister(train);
    
    // Start Train 'GPS' and physics tick
    TaskID tPosGPS = VAL_TO_ID(sysCreate(sysPriority() + 1, &PositionUpdateCourier));
    TaskID tPhysicsTick = VAL_TO_ID(sysCreate(sysPriority() - 1, &PhysicsTick));
    TaskID tRandomSpeed = VAL_TO_ID(sysCreate(sysPriority() - 1, &RandomSpeed));
    TimerState tickTimer;
    TimerState errorTimer;
    timerStart(TIMER_4, &tickTimer);
    timerStart(TIMER_4, &errorTimer);

    TrackNode* previousSensor = 0;
    U8 reverseCourierState = 0;
    TaskID reverseCourier = { 0 };

    S16     throttle = 0;
    //char    nextSensorGroup = 0;
    //U8      nextSensorId = 0;
    U32     predictTime[3] = { 0 };
    U32     stopSensor = 0xFFFF;
    S32     stopDistance = 0;
    U32     stopping = 0;
    U8      setSpeed = 0xFF;
    GPSUpdate previousUpdate;
    GPSUpdate update;
    for(;;)
    {
        sysReceive(&from.value, &env);

        if( from.value == tPhysicsTick.value )
        {
            sysReply(from.value, &env);
            timerSample(TIMER_4, &tickTimer);
            S32 delta = trainPhysicsStep(&physics, tickTimer.delta);
            //printf(strPhysicsTrain, train, physics.targetSpeed, physics.velocity, physics.acceleration);
            
            if (stopDistance > 0)
            {
                U32 stopDist = trainPhysicsStopDist(&physics);
                if (stopDist < stopDistance)
                {
                    stopDistance -= delta;
                    delta /= 2;
                    if ((stopDistance - delta) <= trainPhysicsStopDist(&physics) && stopping == 0)
                    {
                        trainSetSpeed(sTrainDriver, train, 0);
                        trainPhysicsSetSpeed(&physics, 0);
                        stopping = 1;
                        stopSensor = 0xFFFF;
                    }
                }
                else if (stopping == 1)
                {
                    stopDistance -= delta;
                }

                //printf("\033[s\033[43;1HDistance til Stop Sensor %d.\r\n\033[u", stopDistance);
            }
        }
        else if (from.value == tRandomSpeed.value )
        {
            sysReply(from.value, &env);
/*            setSpeed = env.message.MessageU8.body;
            if (physics.velocity == 0)
            {
                trainPhysicsSetSpeed(&physics, setSpeed);
                trainSetSpeed(sTrainDriver, train, setSpeed);
                setSpeed = 0xFF;
            }*/
        }
        else if( from.value == tPosGPS.value )
        {
            previousUpdate = update; 
            update = *(GPSUpdate*)(env.message.MessageArbitrary.body);
            sysReply(tPosGPS.value, &env);
            U8      currentSensor = update.triggeredSensor;
            //char    sensorGroup = 'A' + currentSensor / 16;
            //U8      sensorI = currentSensor % 16 + 1;
 
            timerSample(TIMER_4, &errorTimer);
            
            if(previousSensor != 0)
            {
                timerSample(TIMER_4, &tickTimer);
                trainPhysicsStep(&physics, tickTimer.delta);

                //nextSensorGroup = previousUpdate.prediction[update.sensorSkip] / 16 + 'A';
                //nextSensorId = previousUpdate.prediction[update.sensorSkip] % 16 + 1;
                
                S32 deltaT = predictTime[update.sensorSkip] - errorTimer.delta/1000;
                if (deltaT < -50 || deltaT > 50)
                {
                    //printf(strPredictOldTrain, train, nextSensorGroup, nextSensorId, predictTime[update.sensorSkip]);
                }
                else
                {
                    //printf(strPredictClearTrain);
                }
                
                if( stopSensor < 0xFFFF )
                {
                    TrackNode* ip = graph + previousUpdate.prediction[update.sensorSkip];
                    U32 nodeCount = 0;
                    stopDistance = 0;
                    do
                    {
                        nodeCount++;
                        if(ip->type == eNodeBranch)
                        {
                            MessageEnvelope env;
                            env.message.MessageU32.body = (ip - (graph + 80)) / 2;
                            env.type = MESSAGE_SWITCH_READ;
                            sysSend(sSwitchOffice.value, &env, &env);

                            SwitchState sw = (SwitchState)env.message.MessageU32.body;
                            U32 swv = ((sw == eCurved) ? DIR_CURVED : DIR_STRAIGHT);
                            stopDistance += ip->edge[swv].dist - ip->edge[swv].dx;
                            ip = ip->edge[swv].dest;
                        }
                        else
                        {
                            stopDistance += ip->edge[DIR_AHEAD].dist - ip->edge[DIR_AHEAD].dx;
                            ip = ip->edge[DIR_AHEAD].dest;
                        }
                    } while( nodeCount < 150 && (ip->type != eNodeSensor || ip->num != stopSensor) );
                }

                // Predict
                S32 distance = update.distance;
                S32 traveledDistance = trainPhysicsGetDistance(&physics);
                S32 deltaX = distance - traveledDistance;
              
                TrackNode* ip = previousSensor;
                U32 dist = 0;
                TrackEdge* edgeFrom = 0;
                do
                {
                    if(ip->type == eNodeBranch)
                    {
                        MessageEnvelope env;
                        env.message.MessageU32.body = (ip - (graph + 80)) / 2;
                        env.type = MESSAGE_SWITCH_READ;
                        sysSend(sSwitchOffice.value, &env, &env);

                        SwitchState sw = (SwitchState)env.message.MessageU32.body;
                        U32 swv = ((sw == eCurved) ? DIR_CURVED : DIR_STRAIGHT);
                        edgeFrom = &ip->edge[swv];
                        dist += ip->edge[swv].dist;
                        ip = ip->edge[swv].dest;
                    }
                    else
                    {
                        edgeFrom = &ip->edge[DIR_AHEAD];
                        dist += ip->edge[DIR_AHEAD].dist;
                        ip = ip->edge[DIR_AHEAD].dest;
                    }
                } while( dist < 5000 && ( (ip->type != eNodeSensor) || (ip->num != currentSensor) ) );
                assert(edgeFrom != 0);
                
                if( dist < 5000 )
                {
                    S32 accelReport = trainPhysicsReport(&physics, distance, errorTimer.delta, deltaT);
                    
                    if (setSpeed < 14)
                    {
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

                    //nextSensorGroup = update.prediction[0] / 16 + 'A';
                    //nextSensorId = update.prediction[0] % 16 + 1;

                    U8 i = 0;
                    for(i = 0; i < 3; ++i)
                    {
                        predictTime[i] = trainPhysicsGetTime(&physics, update.predictionDistance[i]) / 1000 - update.time[i];
                    }

                    //printf(strPredictTrain, train, nextSensorGroup,
                    //       nextSensorId, predictTime[0]);

                    //printf(strFoundTrain, train, sensorGroup, sensorId,
                      //     errorTimer.delta/1000, deltaT,
                        //   previousUpdate.time[update.sensorSkip],
                          // distance, traveledDistance, deltaX,
                          // physics.velocity);
                }
            }

            previousSensor = graph + currentSensor;
        }
        else
        {
            switch(env.type)
            {
            case MESSAGE_TRAIN_STOP:
            {
                stopSensor = env.message.MessageU32.body;
                sysReply(from.value, &env);
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

                    stopping = 0;
                    stopDistance = 0;
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
    for(;;)
    {
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
    
    TrackNode* ipFail = 0;
    memset(dist, 0, sizeof(U32)*3);
    memset(time, 0, sizeof(U32)*3);
    memset(nextSensors, 0xFF, sizeof(U32)*3);

    while(ip->type != eNodeExit)
    {
        if(ip->type == eNodeSensor)
        {
            nextSensors[i++] = ip->num; 

            if( i >= 3 ) 
            {
                break;
            }
            else if( i < 2 )
            {
                dist[i] = dist[i-1];
                time[i] = time[i-1];
            }
            else if( ipFail )
            {
                ip = ipFail;
                continue;
            }
            else
            {
                break;
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
            
            // Only look at the first possible failure.
            if(!ipFail) {
                U32 swf = ((sw == eCurved) ? DIR_STRAIGHT: DIR_CURVED);
                dist[2] = dist[i] + ip->edge[swf].dist;
                time[2] = time[i] + ip->edge[swf].dt;
                ipFail = ip->edge[swf].dest;
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
    TrackNode* position = 0;

    U32 sensors[5], nextSensors[3], distances[3], time[3];
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
                    
                    if( !isHit ) continue;
                    
                    if( !position )
                    {
                        sensorHit = 1;
                        nextSensors[0] = c + i * 16;
                        distances[0] = 0;
                    }
                    else
                    {
                        U8 s = 0;
                        for(s = 1; s <= 3; ++s)
                        {
                            if(nextSensors[s - 1] == (c + i * 16))
                            {
                                sensorHit = min(sensorHit, s);
                                break;
                            }
                        }
                    }
                }
            }
            // Its what we predicted!
            if(sensorHit <= 3 && sensorHit >= 1)
            {
                position = graph + nextSensors[sensorHit-1];
                
                env.message.MessageArbitrary.body = (U32*)(&update);
                update.triggeredSensor = nextSensors[sensorHit-1];
                update.sensorSkip = sensorHit-1;
                update.distance = distances[sensorHit-1];

                findNextSensors(graph, position->edge[0].dest, nextSensors, distances, time);
                for(i = 0; i < 3; ++i)
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
