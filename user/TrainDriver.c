#include "kernel/kernel.h"

#include "user/messageTypes.h"
#include "user/NameServer.h"
#include "user/TrainDriver.h"
#include "user/ClockServer.h"
#include "user/terminal.h"

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

void trainReverseDirection(TaskID server, U8 train, U8 newSpeed)
{
    assert( train <= 80 && train >= 1 );
    assert( newSpeed < 15 );

    TaskID clock; nsWhoIs(Clock, &clock);

    trainSetSpeed(server, train, 0);
    clockDelayBy(clock, 200);

    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_REVERSE;
    env.message.MessageU16.body = (train << 8) | 0x0F;
    sysSend(server.value, &env, &env);

    clockDelayBy(clock, 100);
    trainSetSpeed(server, train, newSpeed);
}

void trainSetSpeed(TaskID server, U8 train, U8 speed)
{
    assert( speed < 15 );
    assert( train <= 80 && train >= 1 );

    MessageEnvelope env;
    env.type = MESSAGE_TRAIN_SET_SPEED;
    env.message.MessageU16.body = (train << 8) | speed;
    sysSend(server.value, &env, &env);
}

void trainSwitch(TaskID server, U8 switchAddress, SwitchState sw)
{
    TaskID clock; nsWhoIs(Clock, &clock);
    
    MessageEnvelope env;
    env.type = sw;
    env.message.MessageU8.body = switchAddress;
    sysSend(server.value, &env, &env);

    clockDelayBy(clock, 16);
    trainSolenoidOff(server);
}

static U8 getTrainDriverMessageType(U16 inMsgType)
{
    if( (inMsgType & 0x00FF) <= 0x0F ) return DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE;

    switch(inMsgType & 0x00FF)
    {
    case MESSAGE_TRAIN_SWITCH_STRAIGHT:
    case MESSAGE_TRAIN_SWITCH_CURVED:
    case MESSAGE_TRAIN_SET_SPEED:
    case MESSAGE_TRAIN_REVERSE:
    case MESSAGE_TRAIN_GET_SENSOR:
        return DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE;
    
    case MESSAGE_TRAIN_GO:
    case MESSAGE_TRAIN_STOP:
    case MESSAGE_TRAIN_SOLENOID_OFF:
        return DRIVER_MESSAGE_TX_TRAIN;
    
    default:
        break;
    }
    assert( 0 );
    return 0;
}

void TrainInputNotifier(void);
void TrainOutputNotifier(void);

void TrainDriver(void)
{
    nsRegister(Train);
    
    TaskID output,input;

    input.value = sysCreate(0, &TrainInputNotifier);
    output.value = sysCreate(0, &TrainOutputNotifier);

    MessageEnvelope rcvMessage, commandMsg;
    TaskID rcvID;

    U16 commandBacking[16];
    QueueU16 commands;
    queueU16Init(&commands, commandBacking, 16);
   
    U16 blockedTaskCommandsBacking[16];
    U16 blockedTasksBacking[16];
    QueueU16 blockedTaskCommands;
    QueueU16 blockedTasks;
    queueU16Init(&blockedTaskCommands, blockedTaskCommandsBacking, 16);
    queueU16Init(&blockedTasks, blockedTasksBacking, 16);

    U8 isOutputWaiting = 0;
    TaskID stdio, clock; 
    
    nsWhoIs(TerminalOutput, &stdio);
    nsWhoIs(Clock, &clock);

    while( sysRunning() != 0 )
    {
        sysReceive(&rcvID.value, &rcvMessage);

        switch( rcvMessage.type )
        {
        case MESSAGE_TRAIN_REVERSE:
        case MESSAGE_TRAIN_SET_SPEED:
        {
            rcvMessage.type = rcvMessage.message.MessageU16.body & 0xFF;
            rcvMessage.message.MessageU8.body = rcvMessage.message.MessageU16.body >> 8;
        }

        case MESSAGE_TRAIN_SWITCH_STRAIGHT:
        case MESSAGE_TRAIN_SWITCH_CURVED:
        case MESSAGE_TRAIN_GO:
        case MESSAGE_TRAIN_STOP:
        case MESSAGE_TRAIN_SOLENOID_OFF:
        {
            U16 fullCmd = (rcvMessage.message.MessageU8.body << 8) | rcvMessage.type;
            if( isOutputWaiting )
            {
                commandMsg.type = getTrainDriverMessageType(fullCmd);
                commandMsg.message.MessageU16.body = fullCmd;
                sysReply(output.value, &commandMsg);
                sysReply(rcvID.value, &rcvMessage);
                isOutputWaiting = 0;
            }
            else if( queueU16Push(&commands, fullCmd) >= 0 )
            {
                sysReply(rcvID.value, &rcvMessage);
            }
            else
            {
                queueU16Push(&blockedTaskCommands, fullCmd);
                queueU16Push(&blockedTasks, rcvID.value);
            }
            break;
        } 

        case DRIVER_MESSAGE_TX_TRAIN:
        case DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE:
        {
            if( queueU16Pop(&commands, &commandMsg.message.MessageU16.body) >= 0 )
            {
                commandMsg.type = getTrainDriverMessageType(commandMsg.message.MessageU16.body);
                sysReply(output.value, &commandMsg);

                U16 val;
                if( queueU16Pop(&blockedTaskCommands, &val) >= 0 )
                {
                    U16 id;
                    queueU16Pop(&blockedTasks, &id);
                    rcvMessage.type = val;
                    rcvMessage.message.MessageU32.body = val;
                    sysReply(id, &rcvMessage);
                    queueU16Push(&commands, val);
                }
            }
            else
            {
                isOutputWaiting = 1;
            }
            break;
        }


        default:
            sysReply( rcvID.value, &rcvMessage );
            break;
        }
    }
}

void TrainOutputNotifier(void)
{
    TaskID server, clock;
    MessageEnvelope notif;

    nsWhoIs(Clock, &clock);
    server.value = sysPid();

    notif.type = DRIVER_MESSAGE_TX_TRAIN;
    
    TaskID cout;
    nsWhoIs(TerminalOutput, &cout);
    while( sysRunning() != 0 )
    {
        sysSend(server.value, &notif, &notif);
        
        assert(!(notif.message.MessageU16.body & 0x0080));

        sysWrite(PORT_TRAIN, notif.message.MessageU16.body & 0x00FF);
        if(notif.type == DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE)
            sysWrite(PORT_TRAIN, notif.message.MessageU16.body >> 8);
    }
}

void TrainInputNotifier(void)
{
    TaskID server;
    MessageEnvelope notif;
    notif.type = DRIVER_MESSAGE_RCV_SENSORS;

    server.value = sysPid();
    while( sysRunning() != 0 )
    {
        notif.message.MessageU16.body = sysRead(PORT_TRAIN) << 8;
        notif.message.MessageU16.body |= sysRead(PORT_TRAIN);
        sysSend( server.value, &notif, &notif ); 
    }
}
