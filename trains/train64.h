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

    trainPhysicsAccelMap(physics, 6,
                         -163256,
                         -116837,
                         0,
                         102892,
                         160003,
                         199840,
                         225858,
                         249664,
                         266916,
                         275063);

    trainPhysicsAccelMap(physics, 7,
                         -183256,
                         -204178,
                         -122801,
                         0,
                         145983,
                         165639,
                         236047,
                         301928,
                         3023325,
                         297237);

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

    trainPhysicsAccelMap(physics, 9,
                         -201428,
                         -260017,
                         -225504,
                         -169676,
                         -50000,
                         0,
                         144178,
                         204682,
                         260656,
                         295647);

    trainPhysicsAccelMap(physics, 10,
                         -291428,
                         -288042,
                         -280577,
                         -276003,
                         -240000,
                         -180000,
                              0,
                          91014,
                         160151,
                         202021);

    trainPhysicsAccelMap(physics, 11,
                         -331428,
                         -328042,
                         -300577,
                         -296003,
                         -280000,
                         -210000,
                         -140000,
                              0,
                          79318,
                         247931);

    trainPhysicsAccelMap(physics, 12,
                         -309478,
                         -320728,
                         -313774,
                         -312426,
                         -289475,
                         -262204,
                         -185818,
                         -145809,
                         0,
                         558298);

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
