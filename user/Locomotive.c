#include "kernel/kernel.h"
#include "hardware/hardware.h"

#include "user/services/services.h"
#include "user/messageTypes.h"

#include "user/trains/trains.h"
#include "user/trains/TrainYard.h"
#include "user/trains/TrainCommander.h"

#include "trains/train62.h"
#include "trains/trainPhysics.h"

typedef struct
{
    U32 triggeredSensor;
    U32 sensorSkip;
    U32 distance;
    U32 predictionDistance[2];
    U32 prediction[2];
} GPSUpdate;

static void LocomotiveRadio(void);
static void LocomotiveGPS(void);
static void PositionUpdateCourier(void);

static void PhysicsTick(void)
{
    MessageEnvelope env;

    for(;;)
    {
        clockDelayBy(nsWhoIs(Clock), 3);
        sysSend(sysPid(), &env, &env); 
    }
}

void Locomotive(void)
{
    const char * strPredictOldTrain = "\033[31;7m\033[s\033[44;1H\033[2KLast Prediction:\tTrain %2d | Location %c%2d | Time %dms\033[u\033[m";
    const char * strPredictClearTrain = "\033[s\033[44;1H\033[2K\033[u";
    const char * strPredictTrain = "\033[s\033[48;1H\033[2KNext Prediction:\tTrain %2d | Location %c%2d | Time %dms\033[u";
    const char * strFoundTrain =   "\033[s\033[45;1H\033[2KRecorded Data:\t\tTrain %2d | Location %c%2d | Time %dms | \033[1mDeltaT %dms\033[m\r\n\t\t\tReal Distance: %dmm | Physics Distance: %dmm | DeltaX: %dmm                      \033[u";

    TaskID parent = VAL_TO_ID( sysPid() );
    TaskID from;
    MessageEnvelope env;

    TrackNode graph[TRACK_MAX]; 
    init_tracka(graph);

    // Find out the ID of the train I'm driving around.
    sysReceive(&from.value, &env);
    assert(from.value == parent.value);
    sysReply(from.value, &env);
    U8 train = env.message.MessageU8.body;
   
    assert(sysPriority() > 1);
    sysSend(sysCreate(sysPriority()+1, &LocomotiveRadio), &env, &env);

    // Register ourselves.
    trainRegister(train);
    
    // Start Train 'GPS'
    TaskID tPosGPS = VAL_TO_ID(sysCreate(sysPriority() + 1, &PositionUpdateCourier));

    TaskID sTrainDriver = nsWhoIs(Train);

    // Find Train by moving forward and waiting for a sensor.
    trainSetSpeed(sTrainDriver, train, 0);
    S16 throttle = 0;

    TrainPhysics physics;
    train62(&physics);
    trainPhysicsSetSpeed(&physics, 0);

    TaskID tPhysicsTick = VAL_TO_ID(sysCreate(sysPriority() - 1, &PhysicsTick));

    TimerState tickTimer;
    TimerState errorTimer;
    timerStart(TIMER_4, &tickTimer);
    timerStart(TIMER_4, &errorTimer);

    TrackNode* previousSensor = 0;
    U8 reverseCourierState = 0;
    TaskID reverseCourier = { 0 };

    char    nextSensorGroup = 0;
    U8      nextSensorId = 0;
    U32     predictTime[2] = { 0 };
    GPSUpdate previousUpdate;
    GPSUpdate update;
    for(;;)
    {
        sysReceive(&from.value, &env);

        if( from.value == tPhysicsTick.value )
        {
            timerSample(TIMER_4, &tickTimer);
            trainPhysicsStep(&physics, tickTimer.delta);
            sysReply(from.value, &env);
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

            // No prediction during a reverse!
            if((reverseCourierState == 1) || (reverseCourierState == 3))
            {
                continue;
            }

            if(previousSensor != 0)
            {
                timerSample(TIMER_4, &tickTimer);
                trainPhysicsStep(&physics, tickTimer.delta);

                nextSensorGroup = previousUpdate.prediction[update.sensorSkip] / 16 + 'A';
                nextSensorId = previousUpdate.prediction[update.sensorSkip] % 16 + 1;
                
                S32 delta = errorTimer.delta/1000 - predictTime[update.sensorSkip];
                if (delta < -50 || delta > 50)
                {
                    printf(strPredictOldTrain, train, nextSensorGroup, nextSensorId, predictTime[update.sensorSkip]);
                }
                else
                {
                    printf(strPredictClearTrain);
                }
                
                // Predict
                S32 distance = update.distance;
                S32 traveledDistance = trainPhysicsGetDistance(&physics);
               
                nextSensorGroup = update.prediction[0] / 16 + 'A';
                nextSensorId = update.prediction[0] % 16 + 1;

                if (delta < 200 && delta > -200)
                {
                    trainPhysicsReport(&physics, distance, errorTimer.delta);
                }
                
                U8 i = 0;
                for(i = 0; i < 2; ++i)
                    predictTime[i] = trainPhysicsGetTime(&physics, update.predictionDistance[i]) / 1000;
               
                printf(strPredictTrain, train, nextSensorGroup, nextSensorId, predictTime[0]);
                printf(strFoundTrain, train, sensorGroup, sensorId, errorTimer.delta/1000, delta, distance, traveledDistance, distance-traveledDistance);
            }
            previousSensor = graph + currentSensor;
        }
        else
        {
            switch(env.type)
            {
            case MESSAGE_COURIER:
                reverseCourier = from;
                if( reverseCourierState == 0 )
                {
                    reverseCourierState = 2; // Waiting for a reverse command
                }
                else if(reverseCourierState == 1 )
                {
                    // Reverse was happening and courier missed it.
                    env.type = MESSAGE_TRAIN_REVERSE;
                    sysReply(reverseCourier.value, &env);
                    reverseCourierState = 3;
                    // Now wait for courier to come back 'round.
                }
                else if(reverseCourierState == 3 )
                {
                    // Courier has come back 'round.
                    reverseCourierState = 0;
                }
                break;
            case MESSAGE_TRAIN_SET_SPEED:
                throttle = env.message.MessageU8.body;
                trainPhysicsSetSpeed(&physics, throttle);
                trainSetSpeed(sTrainDriver, train, throttle);
                if (throttle == 0)
                {
                    printf("Stopping dist: %d\r\n", trainPhysicsStopDist(&physics));
                }
                sysReply(from.value, &env);
                break;
            case MESSAGE_TRAIN_REVERSE:
                assert(reverseCourierState == 3); // Can't do command over again.
                if(reverseCourierState == 2)
                {
                    // Courier is waiting for us to send command.
                    sysReply(reverseCourier.value, &env);
                    reverseCourierState = 3; 
                    // Now wait for courier to come back around.
                }
                else if(reverseCourierState == 0)
                {
                    // Courier has missed message.
                    reverseCourierState = 1;
                    // Make sure when courier comes around we send it the reverse command.
                }
 
                trainSetSpeed(sTrainDriver, train, 0);
                clockDelayBy(nsWhoIs(Clock), 20 * (throttle));

                env.message.MessageU16.body = (train << 8) | 0x0F;
                sysSend(sTrainDriver.value, &env, &env);

                clockDelayBy(nsWhoIs(Clock), 5);
                trainSetSpeed(sTrainDriver, train, throttle);
                sysReply(from.value, &env);
                break;
            }
        }
    }
}

static U16 firstSensorTriggered()
{
    TaskID sSensors = nsWhoIs(TrainSensors);
    U16 currentSensor = 0xFFFF;
    do
    {
        U32 sensors[5];
        U8 i = 0;
        
        trainReadAllSensors(sSensors, sensors);  
        
        for(i = 0; i < 5; ++i)
        {
            // Read all sensors
            U8 c = 0;
            U16 group = (U16)sensors[i];
            for(c = 0; c < 16; ++c)
            {
                if((1 << (15-c)) & group)
                {
                    currentSensor = c + 16 * i;
                    break;
                }
            }
        }
    } while( currentSensor == 0xFFFF );
    return currentSensor;
}

static void LocomotiveRadio(void)
{
    TaskID yard = nsWhoIs(TrainYard);
    TaskID loco; 
    
    MessageEnvelope env;
    sysReceive(&loco.value, &env);
    sysReply(loco.value, &env);
    U8 train = env.message.MessageU8.body;
 
    env.type = MESSAGE_NAMESERVER_REGISTER;
    env.message.MessageU8.body = train;
    sysSend(yard.value, &env, &env);
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
    TaskID gps = VAL_TO_ID(sysCreate(sysPriority(), &LocomotiveGPS));
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



static void findNextSensors(TrackNode* graph, TrackNode* current, U32* nextSensors, U32* faultyBranch, U32* dist)
{
    // FORWARD PREDICTION 
    TrackNode* ip = current;
    U8 i = 0;
    
    TrackNode* ipFail = 0;
    dist[0] = dist[1] = dist[2] = 0;
    while(i < 2)
    {
        if(ip->type == eNodeSensor)
        {
            nextSensors[i] = ip->num; 
            dist[i+1] = dist[i];
            ++i;
        }

        if(ip->type == eNodeBranch)
        {
            MessageEnvelope env;
            env.message.MessageU8.body = (ip - (graph + 80)) / 2;
            env.type = MESSAGE_SWITCH_READ;
            sysSend(nsWhoIs(TrainSwitchOffice).value, &env, &env);

            SwitchState sw = (SwitchState)env.message.MessageU8.body;
            U8 swv = ((sw == eCurved) ? DIR_CURVED : DIR_STRAIGHT);
            dist[i] += ip->edge[swv].dist;
            ip = ip->edge[swv].dest;
            
            // Only look at the first possible failure.
            if(!ipFail) {
                dist[3] = dist[i];
                ipFail = ip->edge[DIR_CURVED - swv].dest;
            }
        }
        else
        {
            dist[i] += ip->edge[DIR_AHEAD].dist; 
            ip = ip->edge[DIR_AHEAD].dest;
        }
    }

    if(ipFail)
    {
        ip = ipFail;
        while(ip->type != eNodeSensor)
        {
            if(ip->type == eNodeBranch)
            {
                MessageEnvelope env;
                env.message.MessageU8.body = (ip - (graph + 80)) / 2;
                env.type = MESSAGE_SWITCH_READ;
                sysSend(nsWhoIs(TrainSwitchOffice).value, &env, &env);

                SwitchState sw = (SwitchState)env.message.MessageU8.body;
                U8 swv = ((sw == eCurved) ? DIR_CURVED : DIR_STRAIGHT);
                dist[3] += ip->edge[swv].dist;
                ip = ip->edge[swv].dest;
            }
            else
            {
                dist[3] += ip->edge[DIR_AHEAD].dist;
                ip = ip->edge[DIR_AHEAD].dest;
            }
        }
        faultyBranch[0] = ip->num;
    }
}

static void LocomotiveGPS(void)
{
    TrackNode graph[TRACK_MAX]; 
    init_tracka(graph);

    TrackNode* position = graph + firstSensorTriggered();

    U32 sensors[5], nextSensors[2], faultBranchSensor[1], distances[3];
    U32 i = 0;
 
    assert(sysPriority() < 31);
    TaskID tSensors = VAL_TO_ID(sysCreate(sysPriority() + 1, &LocomotiveSensor));
    TaskID tPosCourier = VAL_TO_ID(sysPid());
    U8 courierWaiting = 0;

    GPSUpdate update;
    findNextSensors(graph, position->edge[0].dest, nextSensors, faultBranchSensor, distances);
    for(;;)
    {
        TaskID from;
        MessageEnvelope env;
        sysReceive(&from.value, &env);

        if( from.value == tSensors.value )
        {
            memcpy(sensors, env.message.MessageArbitrary.body, 5);
            sysReply(tSensors.value, &env);

            U8 sensorHit = 0;
            U8 failHit = 0;
            for(i = 0; i < 5; ++i)
            {
                U8 c = 0;
                for(c = 0; c < 16; ++c)
                {
                    U16 isHit = (sensors[i] & (1 << (15-c)));
                    
                    if( !isHit ) continue;

                    if(nextSensors[0] == (c + i * 16))
                    {
                        sensorHit = 1;
                        break;
                    }
                    else if(nextSensors[1] == (c + i * 16))
                    {
                        sensorHit = 2;
                    }
                    else if(faultBranchSensor[0] == (c + i * 16))
                    {
                    //    failHit = 1;
                    }
                }
                if(sensorHit == 1) break;
            }
            // Its what we predicted!
            if(sensorHit)
            {
                position = graph + nextSensors[sensorHit-1];
                if(courierWaiting)
                {
                    courierWaiting = 0;

                    env.message.MessageArbitrary.body = (U32*)(&update);
                    update.triggeredSensor = nextSensors[sensorHit-1];
                    update.sensorSkip = sensorHit-1;
                    update.distance = distances[sensorHit-1];

                    findNextSensors(graph, position->edge[0].dest, nextSensors, faultBranchSensor, distances);
                    for(i = 0; i < 3; ++i)
                        distances[i] += position->edge[0].dist;
                    
                    for(i = 0; i < 3; ++i)
                    {
                        update.predictionDistance[i] = distances[i];
                        update.prediction[i] = (i == 2 ? faultBranchSensor[0] : nextSensors[i]);
                    }
                    sysReply(tPosCourier.value, &env);
                }
                else
                {
                    findNextSensors(graph, position->edge[0].dest, nextSensors, faultBranchSensor, distances);
                    for(i = 0; i < 3; ++i)
                        distances[i] += position->edge[0].dist;
                }
            }
            else if(failHit)
            {
                position = graph + faultBranchSensor[0];
                if(courierWaiting)
                {
                    courierWaiting = 0;

                    env.message.MessageArbitrary.body = (U32*)(&update);
                    update.triggeredSensor = faultBranchSensor[0];
                    update.sensorSkip = 2;
                    update.distance = distances[2];

                    findNextSensors(graph, position->edge[0].dest, nextSensors, faultBranchSensor, distances);
                    for(i = 0; i < 3; ++i)
                        distances[i] += position->edge[0].dist;
                    
                    for(i = 0; i < 3; ++i)
                    {
                        update.predictionDistance[i] = distances[i];
                        update.prediction[i] = (i == 2 ? faultBranchSensor[0] : nextSensors[i]);
                    }
                    sysReply(tPosCourier.value, &env);
                }
                else
                {
                    findNextSensors(graph, position->edge[0].dest, nextSensors, faultBranchSensor, distances);
                    for(i = 0; i < 3; ++i)
                        distances[i] += position->edge[0].dist;
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
                findNextSensors(graph, position, nextSensors, faultBranchSensor, distances);
                sysReply(from.value, &env);
                break;
            }
            }
        }
    }
}
