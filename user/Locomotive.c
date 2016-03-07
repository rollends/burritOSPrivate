#include "kernel/kernel.h"
#include "user/services/services.h"
#include "user/trackData.h"
#include "user/messageTypes.h"
#include "user/TrainYard.h"

static void LocomotiveSensor(void);
static void LocomotiveRadio(void);

void Locomotive(void)
{
    const char * strFoundTrain = "\033[s\033[43;1HFound Train %2d at Location %c%2d with time %d.\033[u";
    const char * strPredictTrain = "\033[s\033[44;1HPredict Train %2d at Location %c%2d next with time %d.\033[u";

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
    
    // Start Train 'GPS'
    TaskID tGPS = VAL_TO_ID(sysCreate(sysPriority() + 1, &LocomotiveSensor));
    sysSend(tGPS.value, &env, &env);
    TrackNode* graph = (TrackNode*)env.message.MessageArbitrary.body;

    TaskID sTrainDriver = nsWhoIs(Train);
    TaskID sSwitchOffice = nsWhoIs(TrainSwitchOffice);
    TaskID sClock = nsWhoIs(Clock);

    // Find Train by moving forward and waiting for a sensor.
    trainSetSpeed(sTrainDriver, train, 8);
    U32 currentTime = 0;
       
    for(;;)
    {
        sysReceive(&from.value, &env);

        if( from.value == tGPS.value )
        {
            sysReply(tGPS.value, &env);

            U8      currentSensor = env.message.MessageU8.body;
            char    sensorGroup = 'A' + currentSensor / 16,
                    nextSensorGroup = sensorGroup;
            U8      sensorId = currentSensor % 16 + 1,
                    nextSensorId = sensorId;
            currentTime = clockTime(sClock);

            // Result
            printf(strFoundTrain, train, sensorGroup, sensorId, currentTime);

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

            printf(strPredictTrain, train, nextSensorGroup, nextSensorId, 0);
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
    TaskID commander = nsWhoIs(TrainCommander);
    TaskID yard = nsWhoIs(TrainYard);
    TaskID loco; 
    
    MessageEnvelope env;
    sysReceive(&loco.value, &env);
    sysReply(loco.value, &env);
    U8 train = env.message.MessageU8.body;
 
    env.type = MESSAGE_NAMESERVER_REGISTER;
    sysSend(yard.value, &env, &env);
    for(;;)
    {
        sysSend(commander.value, &env, &env);
        sysSend(loco.value, &env, &env);
    }
}

static void LocomotiveSensor(void)
{
    TaskID sSwitchOffice = nsWhoIs(TrainSwitchOffice);
    TaskID sSensors = nsWhoIs(TrainSensors);
    TaskID parent;
    MessageEnvelope env;

    TrackNode graph[TRACK_MAX]; 
    init_tracka(graph);

    sysReceive(&parent.value, &env);
    env.message.MessageArbitrary.body = (U32*)graph;
    sysReply(parent.value, &env);

    TrackNode* position = graph + firstSensorTriggered();
    TrackNode* ip = position;

    U32 sensors[5];
    U32 i = 0;
    U32 const MaxSearchDepth = 3;

    env.message.MessageU8.body = position - graph;
    sysSend(parent.value, &env, &env);
    for(;;)
    {
        trainReadAllSensors(sSensors, sensors);  
        
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
            env.message.MessageU8.body = position - graph;
            sysSend(parent.value, &env, &env);
            continue;
        }
    
        // REVERSE PROJECTION - See if its a sensor behind us?
        ip = position->reverse;
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
            env.message.MessageU8.body = position - graph;
            sysSend(parent.value, &env, &env);
            continue;
        }
    }
}
