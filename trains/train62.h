#ifndef TRAIN_62_H
#define TRAIN_62_H

#include "trains/trainPhysics.h"
/**
 * Initializes a train struct with data for train 62
 *
 * @param   physics     The TrainPhysics data to initialize
 *
*/
void train62(TrainPhysics* physics)
{
    trainPhysicsInit(physics);
    trainPhysicsSpeedMap(physics,
                         224376,
                         275931,
                         335343,
                         389936,
                         441837,
                         491406,
                         541752,
                         589662,
                         632801);

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
}

#endif