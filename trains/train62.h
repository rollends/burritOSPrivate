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
                         133476,
                         169915,
                         222645,
                         275814,
                         334307,
                         387725,
                         440735,
                         487500,
                         537218,
                         585262,
                         633311,
                         633309);
}

#endif