#ifndef TRAIN_69_H
#define TRAIN_69_H

#include "trains/trainPhysics.h"
/**
 * Initializes a train struct with data for train 69
 *
 * @param   physics     The TrainPhysics data to initialize
 *
*/
static inline void trainInit69(TrainPhysics* physics)
{
    trainPhysicsInit(physics);
    trainPhysicsSpeedMap(physics,
                         240599,
                         288252,
                         348817,
                         397355,
                         450668,
                         503937,
                         565002,
                         610482,
                         624548);

    trainPhysicsAccelMap(physics, 0,
                         0,
                         62911,
                         73793,
                         92415,
                         86138,
                         92644,
                         124226,
                         286245,
                         276718,
                         225787);

    trainPhysicsAccelMap(physics, 5,
                         -112558,
                         0,
                         88367,
                         170081,
                         186321,
                         197252,
                         209020,
                         232275,
                         249976,
                         362173);

    trainPhysicsAccelMap(physics, 6,
                         -170256,
                         -111175,
                         0,
                         101612,
                         155278,
                         192018,
                         222605,
                         249664,
                         262618,
                         269419);

    trainPhysicsAccelMap(physics, 7,
                         -203256,
                         -199314,
                         -125385,
                         0,
                         128273,
                         166326,
                         226052,
                         280090,
                         298441,
                         292028);

    trainPhysicsAccelMap(physics, 8,
                         -238366,
                         -257626,
                         -225504,
                         -161635,
                         0,
                         119172,
                         137928,
                         203483,
                         249902,
                         278577);

    trainPhysicsAccelMap(physics, 9,
                         -261428,
                         -254094,
                         -221341,
                         -169676,
                         -74128,
                         0,
                         122947,
                         196340,
                         244638,
                         275425);

    trainPhysicsAccelMap(physics, 10,
                         -297510,
                         -273612,
                         -267173,
                         -259017,
                         -238105,
                         -180000,
                               0,
                           86499,
                          171063,
                          198856);

    trainPhysicsAccelMap(physics, 11,
                         -332175,
                         -325114,
                         -295228,
                         -285160,
                         -280885,
                         -209345,
                         -63549,
                              0,
                          66651,
                         239074);

    trainPhysicsAccelMap(physics, 12,
                         -338209,
                         -322059,
                         -313774,
                         -309889,
                         -285422,
                         -255990,
                         -222537,
                         -146324,
                         0,
                         491802);

    trainPhysicsAccelMap(physics, 13,
                         -342269,
                         -327859,
                         -313774,
                         -300150,
                         -279448,
                         -250770,
                         -253571,
                         -214267,
                         -47416,
                         0);
}

#endif
