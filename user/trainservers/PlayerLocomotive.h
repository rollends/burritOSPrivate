#ifndef PLAYER_LOCOMOTIVE_H
#define PLAYER_LOCOMOTIVE_H

#include "trains/trains.h"

typedef struct
{
    TrackNode graph[TRACK_MAX]; 
    TrainPhysics physics;
    TaskID sTrainServer;
    TaskID sSwitchServer;

    U8 train;
    U8 direction;
    U32 random;

    // Conditions
    U32 isLaunching;
    U32 isStopping;
    U32 isReversing;
    U32 hasConflict;

    // State
    U32 speed;
    TrackNode* sensor;
    
    // Autopilot Features
    U8 isPlayer;
    U8 gotoBranch;
    U32 gotoSensor;
    U32 gotoSpeed;
    U32 shouldStop;
    S32 distanceRequired;
    U32 stopDistance;
    U32 distToStop;
    GraphPath destinationPath;

    // Allocation Features
    ListU32Node aAllocatedTrack[25];
    ListU32 allocatedTrack;
    S32 allocatedDistance;

    // Prediction
    U32 predictSensor[4];
    U32 predictDistance[4];
    U32 predictTime[4];

} LocomotiveState;

void LocomotiveRadio(void);
void LocomotiveSensor(void);
void PhysicsTick(void);

void PlayerLocomotive(void);

void locomotiveThrottle (LocomotiveState* state, U8 speed);
void locomotiveStateInit (LocomotiveState* state, U8 train);
S32 locomotiveAllocateTrack (LocomotiveState* state, S32 distanceRequired, QueueU8* qBranchId, QueueU8* qBranchAction);
void locomotiveStep (LocomotiveState* state, U32 deltaTime);
void locomotiveMakePrediction (LocomotiveState* state);
void locomotiveSensorUpdate (LocomotiveState* state, U32 sensorIndex, U32 deltaTime, U32 errorDelta);

#endif
