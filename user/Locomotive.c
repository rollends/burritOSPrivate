#include "kernel/kernel.h"

#include "user/services/services.h"
#include "user/messageTypes.h"

#include "user/trains/trains.h"
#include "user/trains/TrainYard.h"
#include "user/trains/TrainCommander.h"

static void LocomotiveRadio(void);
static void LocomotiveGPS(void);
static void PositionUpdateCourier(void);

void Locomotive(void)
{
    const char * strFoundTrain =   "\033[s\033[44;1HActual : Train %2d | Location %c%2d | Time %d.\033[u";
    const char * strPredictTrain = "\033[s\033[45;1HPredict: Train %2d | Location %c%2d | Time %d.\033[u";

    TaskID parent = VAL_TO_ID( sysPid() );
    TaskID from;
    MessageEnvelope env;

    TrackNode graph[TRACK_MAX]; 
    init_trackb(graph);

    // Find out the ID of the train I'm driving around.
    sysReceive(&from.value, &env);
    assert(from.value == parent.value);
    sysReply(from.value, &env);
    U8 train = env.message.MessageU8.body;
   
    assert(sysPriority() < 31);
    sysSend(sysCreate(sysPriority()+1, &LocomotiveRadio), &env, &env);

    // Register ourselves.
    trainRegister(train);
    
    // Start Train 'GPS'
    TaskID tPosGPS = VAL_TO_ID(sysCreate(sysPriority() + 1, &PositionUpdateCourier));

    TaskID sTrainDriver = nsWhoIs(Train);
    TaskID sSwitchOffice = nsWhoIs(TrainSwitchOffice);
    TaskID sClock = nsWhoIs(Clock);

    // Find Train by moving forward and waiting for a sensor.
    trainSetSpeed(sTrainDriver, train, 8);
    U32 currentTime = 0;
    S16 throttle = 8;

    for(;;)
    {
        sysReceive(&from.value, &env);

        if( from.value == tPosGPS.value )
        {
            sysReply(tPosGPS.value, &env);

            U8      currentSensor = env.message.MessageU8.body;
            char    sensorGroup = 'A' + currentSensor / 16,
                    nextSensorGroup = sensorGroup;
            U8      sensorId = currentSensor % 16 + 1,
                    nextSensorId = sensorId;
            currentTime = clockTime(sClock);

            // Result
            printf(strFoundTrain, train, sensorGroup, sensorId, currentTime);
            printf(strPredictTrain, train, nextSensorGroup, nextSensorId, 0);

            // Predict
            TrackNode* nextNode = graph + 16 * (sensorGroup - 'A') + (sensorId - 1);
            do
            {
                if(nextNode->type == eNodeBranch)
                {
                    MessageEnvelope env;
                    env.message.MessageU8.body = (nextNode - &graph[80]) / 2 + 1;
                    env.type = MESSAGE_SWITCH_READ;
                    sysSend(sSwitchOffice.value, &env, &env);

                    SwitchState sw = (SwitchState)env.message.MessageU8.body;
                    nextNode = nextNode->edge[sw == eCurved ? DIR_CURVED : DIR_STRAIGHT].dest;
                }
                else
                    nextNode = nextNode->edge[DIR_AHEAD].dest;
            } while( nextNode->type != eNodeSensor );

            nextSensorGroup = nextNode->name[0];
            nextSensorId = (nextNode - graph) % 16 + 1;
        }
        else
        {
            switch(env.type)
            {
            case MESSAGE_TRAIN_SET_SPEED:
                throttle = env.message.MessageU8.body;
                trainSetSpeed(sTrainDriver, train, throttle);
                sysReply(from.value, &env);
                break;
            case MESSAGE_TRAIN_REVERSE:
                trainSetSpeed(sTrainDriver, train, 0);
                clockDelayBy(nsWhoIs(Clock), 100 * (throttle));
                
                env.message.MessageU16.body = (train << 8) | 0x0F;
                sysSend(sTrainDriver.value, &env, &env);

                clockDelayBy(nsWhoIs(Clock), 10);
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

static void PositionUpdateCourier(void)
{
    TaskID gps = VAL_TO_ID(sysCreate(sysPriority(), &LocomotiveGPS));
    TaskID parent = VAL_TO_ID(sysPid());
    for(;;)
    {
        MessageEnvelope env;
        sysSend(gps.value, &env, &env);
        sysSend(parent.value, &env, &env);
    }
}

static void LocomotiveGPS(void)
{
    TaskID sSwitchOffice = nsWhoIs(TrainSwitchOffice);

    TrackNode graph[TRACK_MAX]; 
    init_trackb(graph);

    TrackNode* position = graph + firstSensorTriggered();
    TrackNode* ip = position;

    U32 sensors[5];
    U32 i = 0;
    U32 const MaxSearchDepth = 5;
 
    assert(sysPriority() < 31);
    TaskID tSensors = VAL_TO_ID(sysCreate(sysPriority()+1, &LocomotiveSensor));
    TaskID tPosCourier = VAL_TO_ID(sysPid());
    U8 courierWaiting = 0;
    for(;;)
    {
        TaskID from;
        MessageEnvelope env;
        sysReceive(&from.value, &env);

        if( from.value == tSensors.value )
        {
            memcpy(sensors, env.message.MessageArbitrary.body, 5);
            sysReply(tSensors.value, &env);

            // FORWARD PREDICTION - See if its the sensor ahead that triggered.
            ip = position;
            i = 0;
            while(i < MaxSearchDepth)
            {
                if(ip->type == eNodeSensor)
                {
                    if(sensors[ip->num / 16] & (1 << (15 - ip->num % 16)))
                    {
                        // FOUND IT!
                        break;
                    }
                    else
                    {
                        // nope.
                        ++i;
                    }
                }
                if(ip->type == eNodeBranch)
                {
                    MessageEnvelope env;
                    env.message.MessageU8.body = (ip - &graph[80]) / 2;
                    env.type = MESSAGE_SWITCH_READ;
                    sysSend(sSwitchOffice.value, &env, &env);

                    SwitchState sw = (SwitchState)env.message.MessageU8.body;
                    ip = ip->edge[sw == eCurved ? DIR_CURVED : DIR_STRAIGHT].dest;
                }
                else
                    ip = ip->edge[DIR_AHEAD].dest;
            }

            if(i < MaxSearchDepth)
            {
                position = ip;
                if(courierWaiting)
                {
                    courierWaiting = 0;
                    env.message.MessageU8.body = position - graph;
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
                sysReply(from.value, &env);
                break;
            }
            }
        }
    }
}
