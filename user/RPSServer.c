#include "common/types.h"
#include "common/queue.h"

#include "hardware/hardware.h"

#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/systemCall.h"

#include "user/messageTypes.h"
#include "user/RPSServer.h"
#include "user/Nameserver.h"

static void getTwoPlayers( QueueU16* playerQueue );
static void getPlayerMoves( TaskID* players, 
                            QueueU16* playerQueue, 
                            RPSMessageType* plays );
void RPSServer()
{
    //nsRegister( RPS );  

    U16         playerQueueData[16];
    QueueU16    playerQueue;
    U8          i = 0;

    MessageEnvelope env;
    env.type = MESSAGE_RPS;
    
    queueU16Init( &playerQueue, playerQueueData, 16 );
    do
    {
        TaskID          players[2];
        RPSMessageType  gamePlay[2];

        // Get Two Players
        getTwoPlayers( &playerQueue );
        for(i = 0; i < 2; ++i)
        {
            queueU16Pop( &playerQueue, &players[i].value );
            env.message.MessageU8.body = BeginPlay;
            sysReply( players[i].value, &env );
        }
        
        U8 quit = 0;
        do
        {
            // Get Player Moves
            getPlayerMoves( players, &playerQueue, gamePlay );
        
            if( gamePlay[0] == Quit || gamePlay[1] == Quit )
            {
                // One player wishes to quit! Reply to other player that a Quit happened!
                gamePlay[0] = gamePlay[1] = Quit;
                quit = 1;
            }
            else if( gamePlay[0] == gamePlay[1] )
            {
                gamePlay[0] = gamePlay[1] = PlayerTie;
            }
            else if(   (gamePlay[0] == PlayRock && gamePlay[1] == PlayScissors) 
                    || (gamePlay[0] == PlayScissors && gamePlay[1] == PlayPaper)
                    || (gamePlay[0] == PlayPaper && gamePlay[1] == PlayRock) )
            {
                gamePlay[0] = PlayerWon;
                gamePlay[1] = PlayerLost;
            }
            else
            {
                gamePlay[1] = PlayerWon;
                gamePlay[0] = PlayerLost;
            }

            for(i = 0; i < 2; ++i)
            {
                env.message.MessageU8.body = gamePlay[i];
                sysReply( players[i].value, &env );
            }

            {
                U8 c;
                uartReadByte( UART_2, &c ); 
                printString("\r\n\r\n");
            }
        } while( !quit );
    } while( playerQueue.count );
    
    sysExit();
}

static void getTwoPlayers( QueueU16* playerQueue )
{
    MessageEnvelope env;

    while( playerQueue->count < 2 )
    {
        TaskID id;
        sysReceive( &id.value, &env ); 
    
        if( env.type != MESSAGE_RPS )
        {
            sysReply( id.value, &env );
        }
        else if( env.message.MessageU8.body == Signup )
        {
            queueU16Push( playerQueue, id.value );
        }
        else
        {
            printString("RPS Server: Leaving Client %b to Hang due to Invalid Message State.\r\n", 
                        id.fields.id);
        }
    }
}

static void getPlayerMoves( TaskID* players, QueueU16* playerQueue, RPSMessageType* plays )
{
    MessageEnvelope env;
    U8 numPlays = 0;
    while( numPlays < 2 )
    {
        TaskID id;
        sysReceive( &id.value, &env ); 
        U8 playerIndex = 0;

        if( players[0].value == id.value )
        {
            playerIndex = 0;
        }
        else if( players[1].value == id.value )
        {
            playerIndex = 1;
        }
        else
        {
            if( env.message.MessageU8.body == Signup )
            {
                queueU16Push( playerQueue, id.value );
            }
            else
            {
                printString("RPS Server: Leaving Client %b to Hang due to Invalid Message State.\r\n", id.fields.id);
            }
            continue;
        }
    
        plays[playerIndex] = env.message.MessageU8.body;
        numPlays++;     
    }
}
