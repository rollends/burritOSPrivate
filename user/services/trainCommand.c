#include "common/string.h"
#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/SwitchOffice.h"

static U8 trainSpeedHack[80];



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

    TaskID clock = nsWhoIs(Clock);

    U8 oldSpeed = trainSpeedHack[train];

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
    assert( speed < 15 );
    assert( train <= 80 && train >= 1 );

    trainSpeedHack[train] = speed;

    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_SET_SPEED;
    env.message.MessageU16.body = (train << 8) | speed;
    sysSend(server.value, &env, &env);
}

U8 trainReadSensorGroup(TaskID sensorServer, U8 sensorGroup)
{
    assert( sensorGroup > 0 && sensorGroup <= 32 );

    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_GET_SENSOR;
    env.message.MessageU8.body = sensorGroup;
    sysSend(sensorServer.value, &env, &env);
    return env.message.MessageU16.body;
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
