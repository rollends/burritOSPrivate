#include "common/common.h"
#include "kernel/kernel.h"
#include "hardware/hardware.h"
#include "trains/trains.h"

#include "user/display/LogDisplay.h"
#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"
#include "user/trainservers/Locomotive.h"

void PlayerLocomotive(void)
{
//    const char * strPredictOldTrain = "\033[s\033[31;7m\033[s\033[44;1H\033[2KLast Prediction:\tTrain %2d | Location %c%2d | Time %dkt\033[u\033[m\033[u";
//    const char * strPredictClearTrain = "\033[s\033[44;1H\033[2K\033[u";
 //   const char * strFoundTrain =   "\033[s\033[45;1H\033[2KRecorded Data:\t\tTrain %2d | Location %c%2d | Time %dkt | \033[1mDeltaT %dkt\033[m | Correction: %dkt\r\n\t\t\tReal Distance: %dmm | Physics Distance: %dmm | DeltaX: %dmm                \033[u";
    
    assert(sysPriority(sysTid()) < 31);

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
    state.isPlayer = 1;

    // Train Subtasks.
    TaskID tPhysicsTick = VAL_TO_ID(0);
    TaskID tSensor = VAL_TO_ID(sysCreate(sysPriority(sysTid()) - 1, LocomotiveSensor));

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

                TaskID tRadio = VAL_TO_ID(sysCreate(sysPriority(sysTid()) + 1, &LocomotiveRadio));
                env.message.MessageU8.body = train;
                sysSend(tRadio.value, &env, &env);
                
                tPhysicsTick = VAL_TO_ID(sysCreate(sysPriority(sysTid()) - 1, &PhysicsTick));
            }

            locomotiveSensorUpdate(&state, sensorHit - 1, tickTimer.delta, errorTimer.delta);
        }
        else
        {
            switch(env.type)
            {
            case MESSAGE_RPS:
                env.message.MessageArbitrary.body = (U32*)state.graph;
                sysReply(from.value, &env);
                break;

            case MESSAGE_TRAIN_SET_SPEED:
            {
                sysReply(from.value, &env);
                U8 throttle = env.message.MessageU8.body;
                if (!state.speed)
                {
                    locomotiveThrottle(&state, throttle);   
                }
                else
                {
                    state.gotoSpeed = throttle;
                }
                break;
            }

            case MESSAGE_TRAIN_REVERSE:
            {
                sysReply(from.value, &env);
                if (state.speed)
                {
                    state.gotoSpeed = 0;
                    state.isReversing = 1;
                }
                else
                {
                    MessageEnvelope env;
                    env.type = MESSAGE_TRAIN_REVERSE;
                    env.message.MessageU16.body = (state.train << 8) | 0x0F;
                    sysSend(state.sTrainServer.value, &env, &env);
                    state.sensor = state.sensor->reverse;
                    state.physics.distance = 0;
                    locomotiveMakePrediction(&state);
                }
                break;
            }

            default:
                sysReply(from.value, &env);
                break;
            }
        }
    }
}
