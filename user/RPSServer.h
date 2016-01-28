#ifndef RPSSERVER_H
#define RPSSERVER_H

typedef enum
{
    Signup          = 0,
    PlayRock        = 1,
    PlayScissors    = 2,
    PlayPaper       = 3,
    PlayerWon       = 4,
    PlayerLost      = 5,
    PlayerTie       = 6,
    BeginPlay       = 7,
    Quit            = 8,
} RPSMessageType;

void RPSServer();

#endif
