#include "common/common.h"
#include "kernel/kernel.h"
#include "hardware/hardware.h"
#include "trains/trains.h"

#include "user/display/LogDisplay.h"
#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"
#include "user/trainservers/Locomotive.h"

static void LocomotiveRadio(void);
static void LocomotiveSensor(void);
static void PhysicsTick(void);

void Locomotive(void)
{
//    const char * strPredictOldTrain = "\033[s\033[31;7m\033[s\033[44;1H\033[2KLast Prediction:\tTrain %2d | Location %c%2d | Time %dkt\033[u\033[m\033[u";
//    const char * strPredictClearTrain = "\033[s\033[44;1H\033[2K\033[u";
 //   const char * strFoundTrain =   "\033[s\033[45;1H\033[2KRecorded Data:\t\tTrain %2d | Location %c%2d | Time %dkt | \033[1mDeltaT %dkt\033[m | Correction: %dkt\r\n\t\t\tReal Distance: %dmm | Physics Distance: %dmm | DeltaX: %dmm                \033[u";
    
    assert(sysPriority() < 31);

    TaskID from;
    MessageEnvelope env;

    // Find out the ID of the train I'm driving around.
    sysReceive(&from.value, &env);
    sysReply(from.value, &env);
    U8 train = env.message.MessageU8.body;
   
    // Register ourselves.
    trainRegister(train);

    // Initialize State
    LocomotiveState state;
    locomotiveStateInit(&state, train);
 
    // Train Subtasks.
    TaskID tPhysicsTick = VAL_TO_ID(0);
    TaskID tSensor = VAL_TO_ID(sysCreate(sysPriority() - 1, LocomotiveSensor));

    // Timing steps 
    TimerState tickTimer;
    TimerState errorTimer;
    timerStart(TIMER_4, &tickTimer);
    timerStart(TIMER_4, &errorTimer);

    // Launch
    locomotiveThrottle(&state, 5);
    for(;;)
    {
        sysReceive(&from.value, &env);

        if( from.value == tPhysicsTick.value )
        {
            sysReply(from.value, &env);
            timerSample(TIMER_4, &tickTimer);

            locomotiveStep(&state, tickTimer.delta);
        }
        else if( from.value == tSensor.value )
        {
            U32 sensors[5];
            memcpy(sensors, env.message.MessageArbitrary.body, 5);
            sysReply(tSensor.value, &env);

            U8 sensorHit = 0xFF;
            U8 i = 0;

            timerSample(TIMER_4, &errorTimer);
            timerSample(TIMER_4, &tickTimer);
            
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

                    if( !state.sensor )
                    {
                        sensorHit = 1;
                        state.predictSensor[0] = v;
                        state.predictDistance[0] = 0;
                        state.predictTime[0] = 0;
                    }
                    else
                    {
                        U8 s = 0;
                        for(s = 1; s <= 4; ++s)
                        {
                            if(state.predictSensor[s - 1] == v)
                            {
                                sensorHit = min(sensorHit, s);
                                break;
                            }
                        }
                    }
                }
            }

            // Its not our sensor, continue
            if(sensorHit > 4 || sensorHit < 1)
                continue;
                
            if (state.isLaunching)
            {
                logMessage("[Train %d] Found myself!", state.train);

                state.isLaunching = 0;
                locomotiveThrottle(&state, 0);

                TaskID tRadio = VAL_TO_ID(sysCreate(sysPriority() + 1, &LocomotiveRadio));
                env.message.MessageU8.body = train;
                sysSend(tRadio.value, &env, &env);
                
                tPhysicsTick = VAL_TO_ID(sysCreate(sysPriority() - 1, &PhysicsTick));
            }

            locomotiveSensorUpdate(&state, sensorHit - 1, tickTimer.delta, errorTimer.delta);
            //printf(strFoundTrain, train, sensorGroup, sensorId,
             //      errorTimer.delta/1000, deltaT,
             //      previousUpdate.time[update.sensorSkip],
             //      distance, traveledDistance, deltaX,
             //      physics.velocity);
        }
        else
        {
            switch(env.type)
            {
            case MESSAGE_RPS:
                env.message.MessageArbitrary.body = (U32*)state.graph;
                sysReply(from.value, &env);
                break;

            case MESSAGE_TRAIN_GOTO:
            {
                sysReply(from.value, &env);
                state.gotoSensor = env.message.MessageU32.body;
                state.shouldStop = 0;
                assert( pathFind(state.graph, state.sensor->num, state.gotoSensor, &state.destinationPath) >= 0 );
                locomotiveThrottle(&state, 11);
                break;
            }

            case MESSAGE_TRAIN_STOP:
            {
                sysReply(from.value, &env);
                state.gotoSensor = env.message.MessageU32.body;
                state.shouldStop = 1;
                assert( pathFind(state.graph, state.sensor->num, state.gotoSensor, &state.destinationPath) >= 0 );
                locomotiveThrottle(&state, 11);
                break;
            }

            case MESSAGE_TRAIN_SET_SPEED:
            {
                sysReply(from.value, &env);
                U8 throttle = env.message.MessageU8.body;
                if (state.physics.velocity == 0)
                {
                    locomotiveThrottle(&state, throttle);   
                }
                else
                {
                    state.gotoSpeed = throttle;
                }

                if (throttle != 0)
                {
                    if (state.isStopping == 1)
                    {
                        state.sensor = 0;
                    }
                    //stopping = 0;
                    //stopDistance = 0;
                }
                break;
            }

            case MESSAGE_TRAIN_DUMP_ACCEL:
            {
                sysReply(from.value, &env);
                U8 dump = env.message.MessageU8.body;
                U8 i;
                for (i = 0; i < 14; i++)
                {
                    printf("\033[s\033[%d;30H \033[2K %d \033[u", i + 9, state.physics.accelMap[dump][i]);
                }
                break;
            }
            case MESSAGE_TRAIN_DUMP_VEL:
            {
                sysReply(from.value, &env);
                U8 i;
                for (i = 0; i < 14; i++)
                {
                    printf("\033[s\033[%d;45H \033[2K %d \033[u", i + 9, state.physics.speedMap[i]);
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

static void PhysicsTick(void)
{
    MessageEnvelope env;
    TaskID clock = nsWhoIs(Clock);
    
    for(;;)
    {
        clockDelayBy(clock, 2);
        sysSend(sysPid(), &env, &env); 
    }
}


