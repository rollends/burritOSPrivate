#ifndef TRAIN_PHYSICS_H
#define TRAIN_PHYSICS_H

#include "common/common.h"
#include "trains/trainAcceleration.h"

typedef struct
{
    /// Mapping of train speed to steady state velocities
    S32 speedMap[14];

    /// Mapping of train speed to accelerations
    TrainAcceleration accelMap[14];

    /// The current velocity of the train
    S32 velocity;

    /// The current acceleration of the train
    S32 acceleration;

    /// The target velocity of the train
    S32 targetVelocity;

    /// The current speed of the train
    U8  speed;

    /// The target speed of the train
    U8  targetSpeed;
} TrainPhysics;

/**
 * Initials a train physics struct
 *
 * @param   physics     The TrainPhysics struct to initialize
 */
void trainPhysicsInit(TrainPhysics* physics);

/**
 * Initializes a train speed map
 *
 * @param   physics     The TrainPhysics struct to set the speed map
 * @param   v3..v14     Speed map entries
 */
void trainPhysicsSpeedMap(TrainPhysics* physics, 
                          const S32 v3, const S32 v4, const S32 v5,
                          const S32 v6, const S32 v7, const S32 v8,
                          const S32 v9, const S32 v10, const S32 v11,
                          const S32 v12, const S32 v13, const S32 v14);

/**
 * Initializes a train acceleration map entry
 *
 * @param   physics   The TrainPhysics struct to set the accel map
 * @param   speed     The map entry speed
 * @param   m         The map m value
 * @param   b         The map b value
 */
void trainPhysicsAccelMap(TrainPhysics* physics,
                          const U8 speed,
                          const S32 m,
                          const S32 b);

/**
 * Steps the physics simulation by a time delta
 *
 * @param   physics   The TrainPhysics simulation
 * @param   delta     The time delta, in ticks
 */
void trainPhysicsStep(TrainPhysics* physics, const U32 delta);

/**
 * Sets the desired train speed, as sent to the train controller
 *
 * @param   physics   The TrainPhysics struct containing physics data
 * @param   speed     The speed, from 0 to 14
 */
void trainPhysicsSetSpeed(TrainPhysics* physics, const U8 speed);

/**
 * Reports empirical measurements
 *
 * @param   physics   The TrainPhysics struct containing physics data
 * @param   dist      The reported distance
 * @param   ticks     The reported ticks
 */
void trainPhysicsReport(TrainPhysics* physics,
                        const S32 dist,
                        const U32 ticks);

/**
 * Gets the current velocity of the train
 *
 * @param   physics   The TrainPhysics struct containing physics data
 * 
 * @return  The velocity of the train in cm/ktick
 */
S32 trainPhysicsGetVelocity(TrainPhysics* physics);

/**
 * Gets the time taken to traverse a distance given the current state
 *
 * @param   physics   The TrainPhysics struct containing physics data
 * @param   dx        The distance to traverse
 *
 * @return  The time in ticks
 */
U32 trainPhysicsGetTime(TrainPhysics* physics, const S32 dx);

#endif
