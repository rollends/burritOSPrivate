#ifndef TRAIN_ACCELERATION_H
#define TRAIN_ACCELERATION_H

#include "common/common.h"

/**
 * Mapping between current velocity and acceleration to target velocity
 * Uses a linear mapping of the form a = m*v + b, where v is the current
 * velocity and a is the acceleration.
 */ 
typedef struct
{
    /// The m coefficient
    S32 m;

    /// The b coefficient
    S32 b;
} TrainAcceleration;

/**
 * Initializes a TrainAcceleration structure
 *
 * @param   accel   The TrainAcceleration struct
 * @param   m       The m parameter for the mapping
 * @param   b       The b parameter for the mapping
 */
static inline void trainAccelerationInit(TrainAcceleration* accel,
                                         const S32 m,
                                         const S32 b)
{
    accel->m = 0;
    accel->b = 0;
}

/**
 * Computes train acceleration from a current velocity
 *
 * @param   accel   The TrainAcceleration struct
 * @param   v       The current velocity
 *
 * @return  The acceleration
 */
static inline S32 trainAccelerationGet(TrainAcceleration* accel, S32 v)
{
    return accel->m * v + accel->b;
}

#endif
