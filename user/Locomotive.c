#include "kernel/kernel.h"
#include "hardware/hardware.h"

#include "user/services/services.h"
#include "user/messageTypes.h"

#include "user/trains/trains.h"
#include "user/trains/TrainYard.h"
#include "user/trains/TrainCommander.h"

#include "trains/train62.h"
#include "trains/trainPhysics.h"

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
    const char * strPredictTrain = "\033[s\033[44;1H\033[2KPredict: Train %2d | Location %c%2d | Time %d. Velocity %d.\033[u";
    const char * strFoundTrain =   "\033[s\033[45;1H\033[2KActual : Train %2d | Location %c%2d | Time %d. Delta %d.\033[u";
    const char * strFoundTrainDist =   "\033[s\033[46;1H\033[2KTravelled : Train %2d | Distance %d | Expected %d\033[u";
    //const char * strFoundTrain =   "Actual : Train %2d | Location %c%2d | Time %d. Delta %d.\r\n";
    //const char * strPredictTrain = "Predict: Train %2d | Location %c%2d | Time %d.\r\n";


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
//    TaskID sClock = nsWhoIs(Clock);

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

    U32 previousSensor = 0;
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
            sysReply(tPosGPS.value, &env);

            U8      currentSensor = env.message.MessageU8.body;
            char    sensorGroup = 'A' + currentSensor / 16,
                    nextSensorGroup = sensorGroup;
            U8      sensorId = currentSensor % 16 + 1,
                    nextSensorId = sensorId;
            
            if(previousSensor != 0)
            {
                // Predict
                TrackNode* nextNode = graph + previousSensor;
                U32 distance = 0;
                do
                {
                    if(nextNode->type == eNodeBranch)
                    {
                         
                         MessageEnvelope env;
                         env.message.MessageU8.body = (nextNode - &graph[80]) / 2;
                         env.type = MESSAGE_SWITCH_READ;
                         
                         sysSend(sSwitchOffice.value, &env, &env);
                        
                         SwitchState sw = (SwitchState)env.message.MessageU8.body;
                         U8 swv = (sw == eCurved ? DIR_CURVED : DIR_STRAIGHT);
                         distance += nextNode->edge[swv].dist;
                         nextNode = nextNode->edge[swv].dest;
                    }
                    else
                    {
                        distance += nextNode->edge[DIR_AHEAD].dist;
                        nextNode = nextNode->edge[DIR_AHEAD].dest;
                    }
                } while( nextNode->type != eNodeSensor || nextNode->num != currentSensor );
               
                timerSample(TIMER_4, &errorTimer);
                U32 predictTime = trainPhysicsGetTime(&physics, distance) / 1000;
                U32 actualTime = errorTimer.delta / 1000;

                U32 delta = actualTime - predictTime;
                if (predictTime > actualTime)
                    delta = predictTime - actualTime;

                timerSample(TIMER_4, &tickTimer);
                trainPhysicsStep(&physics, tickTimer.delta);

                S32 dist = trainPhysicsGetDistance(&physics);
                printf(strFoundTrainDist, train, dist, distance);
                trainPhysicsReport(&physics, distance, errorTimer.delta);
                printf(strPredictTrain, train, nextSensorGroup, nextSensorId, predictTime, trainPhysicsGetVelocity(&physics));
                printf(strFoundTrain, train, sensorGroup, sensorId, actualTime, delta);
                
                nextSensorGroup = nextNode->name[0];
                nextSensorId = (nextNode - graph) % 16 + 1;
            }
            previousSensor =currentSensor;
        }
        else
        {
            switch(env.type)
            {
            case MESSAGE_TRAIN_SET_SPEED:
                throttle = env.message.MessageU8.body;
                trainPhysicsSetSpeed(&physics, throttle);
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
    U32 const MaxSearchDepth = 15;
 
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
                    ip = ip->edge[(sw == eCurved) ? DIR_CURVED : DIR_STRAIGHT].dest;
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
