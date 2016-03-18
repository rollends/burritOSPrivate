#ifndef TRAIN_64_H
#define TRAIN_64_H

#include "trains/trainPhysics.h"
/**
 * Initializes a train struct with data for train 62
 *
 * @param   physics     The TrainPhysics data to initialize
 *
*/
static inline void trainInit64(TrainPhysics* physics)
{
    trainPhysicsInit(physics);
    trainPhysicsSpeedMap(physics,
                         233899,
                         291604,
                         345865,
                         398276,
                         459456,
                         509406,
                         574540,
                         620500,
                         654256);

    trainPhysicsAccelMap(physics, 0,
                         0,
                         65482,
                         73793,
                         80448,
                         87609,
                         94291,
                         100269,
                         105954,
                         111151,
                         117025);

    trainPhysicsAccelMap(physics, 5,
                         -113256,
                         0,
                         92468,
                         177639,
                         192717,
                         205884,
                         213830,
                         237231,
                         255761,
                         252275);

    trainPhysicsAccelMap(physics, 8,
                         -201428,
                         -260017,
                         -225504,
                         -169676,
                         0,
                         123071,
                         144178,
                         204682,
                         260656,
                         295647);

    trainPhysicsAccelMap(physics, 13,
                         -309478,
                         -320728,
                         -313774,
                         -312426,
                         -289475,
                         -262204,
                         -258886,
                         -145809,
                         -57208,
                         0);
}

#endif
