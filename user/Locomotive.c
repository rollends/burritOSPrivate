#include "kernel/kernel.h"
#include "user/services/services.h"
#include "user/trackData.h"

static U16 waitForTrain()
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

void Locomotive(void)
{
    const char * strFoundTrain = "\033[s\033[43;1HFound Train %2d at Location %c%2d.\033[u";
    const char * strPredictTrain = "\033[s\033[44;1HPredict Train %2d at Location %c%2d next.\033[u";

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
    
    TaskID sTrainDriver = nsWhoIs(Train);
    
    // Find Train by moving forward and waiting for a sensor.
    trainSetSpeed(sTrainDriver, train, 8);
    U16 currentSensor = waitForTrain();
    //trainSetSpeed(sTrainDriver, train, 0);

    for(;;)
    {
        char sensorGroup = 'A' + currentSensor / 16,
             nextSensorGroup = sensorGroup;
        U8   sensorId = currentSensor % 16 + 1,
             nextSensorId = sensorId;

        // Result
        printf( strFoundTrain, 
                train, 
                sensorGroup, 
                sensorId
        );

        // Predict
        TrackNode* nextNode = graph + 16 * (sensorGroup - 'A') + (sensorId - 1);
        do
        {
            if(nextNode->type == eNodeBranch)
            {
                // assume curved cuz lazy.
                nextNode = nextNode->edge[DIR_CURVED].dest;
            }
            else
            {
                nextNode = nextNode->edge[DIR_AHEAD].dest;
            }
        } while( nextNode->type != eNodeSensor );

        nextSensorGroup = nextNode->name[0];
        nextSensorId = (nextNode - graph) % 16 + 1;

        printf( strPredictTrain, 
                train, 
                nextSensorGroup, 
                nextSensorId
        );

        // Wait
        currentSensor = waitForTrain();     
    }
}
