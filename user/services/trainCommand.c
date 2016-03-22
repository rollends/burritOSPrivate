#include "common/string.h"
#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/trainservers/trainservices.h"

S32 dispatchSystemCommand(String string)
{
    U32 len = strlen(string);

  if(len <= 1) { return -1; }

    strtolower(string);
    ConstString cstring = (ConstString)string;

    if( cstring[0] == 's' && cstring[1] == 'w' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 switchId = stratoui(&cstring);
        strskipws(&cstring);
        SwitchState sw = eStraight;
        switch(*cstring)
        {
        case 'c':
            sw = eCurved;
        case 's':
            break;

        default:
            return -1;
        }
        SwitchRequest request;
        request.startTime = clockTime(nsWhoIs(Clock)) + 10;
        request.endTime = request.startTime + 300;
        request.direction = sw;
        request.branchId = switchId;
        request.indBranchNode = 79 + (2 * switchId - 1); 
        MessageEnvelope env;
        env.type = MESSAGE_SWITCH_ALLOCATE;
        env.message.MessageArbitrary.body = (U32*)&request;
        sysSend(nsWhoIs(TrainSwitchOffice).value, &env, &env);
    }
    else if( cstring[0] == 's' && cstring[1] == 'd' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 switchId = stratoui(&cstring);
        strskipws(&cstring);
        SwitchState sw = eStraight;
        switch(*cstring++)
        {
        case 'c':
            sw = eCurved;
        case 's':
            break;

        default:
            return -1;
        }
        strskipws(&cstring);
        U32 timeSeconds = stratoui(&cstring);
 
        SwitchRequest request;
        request.startTime = clockTime(nsWhoIs(Clock)) + timeSeconds * 100;
        request.endTime = request.startTime + 300;
        request.direction = sw;
        request.branchId = switchId;
        request.indBranchNode = 79 + (2 * switchId - 1); 
        MessageEnvelope env;
        env.type = MESSAGE_SWITCH_ALLOCATE;
        env.message.MessageArbitrary.body = (U32*)&request;
        sysSend(nsWhoIs(TrainSwitchOffice).value, &env, &env);
    }
    else
    {
        if( pushTrainCommand(string) < 0 )
            return -1;
    } 
    return 0;
}

void trainStop(TaskID server)
{
    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_STOP;
    sysSend(server.value, &env, &env);
}

void trainGo(TaskID server)
{
    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_GO;
    sysSend(server.value, &env, &env);
}

void trainSolenoidOff(TaskID server)
{
    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_SOLENOID_OFF;
    sysSend(server.value, &env, &env);
}

void trainReverseDirection(TaskID server, U8 train)
{
    assert( train <= 80 && train >= 1 );
    assert(0);

    TaskID clock = nsWhoIs(Clock);

    U8 oldSpeed = 0;// trainSpeedHack[train];

    trainSetSpeed(server, train, 0);
    clockDelayBy(clock, 100 + oldSpeed * 15);

    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_REVERSE;
    env.message.MessageU16.body = (train << 8) | 0x0F;
    sysSend(server.value, &env, &env);

    trainSetSpeed(server, train, oldSpeed);
}

void trainSetSpeed(TaskID server, U8 train, U8 speed)
{
    assert( train <= 80 && train >= 1 );

    //trainSpeedHack[train] = speed;

    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_SET_SPEED;
    env.message.MessageU16.body = (train << 8) | speed;
    sysSend(server.value, &env, &env);
}

U8 trainWaitForSensor(TaskID sensorServer, U8 sensorId)
{
    assert( sensorId < 80 );

    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_GET_SENSOR;
    env.message.MessageU8.body = sensorId;
    sysSend(sensorServer.value, &env, &env);
    return sensorId;
}

void trainReadAllSensors(TaskID sensorServer, U32* sensorArray)
{
    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_GET_SENSOR_ABCDE;
    env.message.MessageArbitrary.body = sensorArray;
    sysSend(sensorServer.value, &env, &env);
}

void trainSwitch(TaskID switchServer, U8 switchAddress, SwitchState sw)
{
    MessageEnvelope env;
    env.type = sw;
    env.message.MessageU8.body = switchAddress;
    sysSend(switchServer.value, &env, &env);
}
