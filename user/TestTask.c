#include "common/random.h"

#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"

#include "user/messageTypes.h"
#include "user/Nameserver.h"
#include "user/TestTask.h"
#include "user/RPSServer.h"

static void gameSignup( TaskID sid );
static void gameQuit( TaskID sid );
static U8 gamePlay( TaskID me, TaskID sid, RPSMessageType move );
static U8 receiveSeed( );

void TestTask()
{
    TaskID rps;
    nsWhoIs( RPS, &rps );
    
    U8 seed = receiveSeed();
    U8 maxPlays = 1 + (nextRandU8( &seed ) & 7);
    TaskID meId;
    meId.value = sysTid();

    printString( "%b: Playing max of %b.\r\n", meId.fields.id, maxPlays, rps.fields.id );
    
    gameSignup( rps );
    while( maxPlays-- > 0 )
    {
        volatile U8 r3 = ( nextRandU8( &seed ) );
		r3 -= 3 * (r3 / 3);

        RPSMessageType move = PlayRock + r3;
        if( gamePlay( meId, rps, move ) )
        {
            goto GameOver;
        }
    }
    gameQuit( rps );

GameOver:
    sysExit();
}

static void gameSignup( TaskID sid )
{
    MessageEnvelope env;
    env.type = MESSAGE_RPS;
    env.message.MessageU8.body = Signup;
    sysSend( sid.value, &env, &env );
}

static void gameQuit( TaskID sid )
{
    MessageEnvelope env;
    env.type = MESSAGE_RPS;
    env.message.MessageU8.body = Quit;
    sysSend( sid.value, &env, &env );
}

static U8 gamePlay( TaskID me, TaskID sid, RPSMessageType move )
{
    MessageEnvelope env;
    env.type = MESSAGE_RPS;
    env.message.MessageU8.body = move;

    sysSend( sid.value, &env, &env );

    printString("%b: Made move ", me.fields.id );
    switch( move )
    {
    case PlayRock:
        printString("rock and ");
        break;
    case PlayScissors:
        printString("scissors and ");
        break;
    case PlayPaper:
        printString("paper and ");
        break;
    default:
        break;
    }

    switch( env.message.MessageU8.body )
    {
    case PlayerWon:
        printString( "I won!\r\n" );
        break;
    case PlayerLost:
        printString( "I lost :'(....\r\n");
        break;
    case PlayerTie:
        printString( "We tied...\r\n" );
        break;
    case Quit:
        printString( "My opponent ditched me :P. I'm just too pro ;). I'll stop playing now.\r\n" );
        return 1;
    }
    return 0;
}

static U8 receiveSeed( )
{
    MessageEnvelope env;
    TaskID id;
    TaskID pid;
    
    pid.value = sysPid();
    do
    {
        sysReceive( &id.value, &env );
        sysReply( id.value, &env );
    } while( id.value != pid.value );

    return env.message.MessageU8.body;
}
