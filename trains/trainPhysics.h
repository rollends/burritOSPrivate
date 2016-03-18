#ifndef TRAIN_PHYSICS_H
#define TRAIN_PHYSICS_H

#include "common/common.h"

typedef struct
{
    /// Mapping of train speed to steady state velocities
    S32 speedMap[14];
    
    /// Calibration count per speed
    S32 calibrationCount[14];

    /// Mapping of train speed to accelerations
    S32 accelMap[14][14];

    /// The current velocity of the train
    S32 velocity;

    /// The velocity at the last report
    S32 initialVelocity;

    /// The speed at the last set
    S32 initialSpeed;

    /// The current acceleration of the train
    S32 acceleration;

    /// The target velocity of the train
    S32 targetVelocity;

    /// The distance since the last report
    S32 distance;

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
 * @param   v5..v14     Speed map entries
 */
void trainPhysicsSpeedMap(TrainPhysics* physics, 
                          const S32 v5, const S32 v6, const S32 v7,
                          const S32 v8, const S32 v9, const S32 v10,
                          const S32 v11, const S32 v12, const S32 v13);
/**
 * Initializes a train speed map
 *
 * @param   physics     The TrainPhysics struct to set the speed map
 * @param   speed       The speed at which to map the accelerations
 * @param   a5..a14     Acceleration map entries
 */
void trainPhysicsAccelMap(TrainPhysics* physics,
                          const S32 speed,
                          const S32 v0, const S32 v5, const S32 v6,
                          const S32 v7, const S32 v8, const S32 v9,
                          const S32 v10, const S32 v11, const S32 v12,
                          const S32 v13);

/**
 * Steps the physics simulation by a time delta
 *
 * @param   physics   The TrainPhysics simulation
 * @param   delta     The time delta, in ticks
 */
U32 trainPhysicsStep(TrainPhysics* physics, const U32 delta);

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
S32 trainPhysicsReport(TrainPhysics* physics,
                        const S32 dist,
                        const U32 ticks,
                        const S32 tickError);

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

S32 trainPhysicsStopDist(TrainPhysics* physics);

/**
 * Gets the distance traveled since the last sensor report
 *
 *
 * @param  physics   The TrainPhysics data struct
 * 
 * @return  The distance traveled
 */
static inline S32 trainPhysicsGetDistance(TrainPhysics* physics)
{
    return physics->distance;
}

#endif
