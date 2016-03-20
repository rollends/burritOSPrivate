#ifndef TRAIN_64_H
#define TRAIN_64_H

#include "trains/trainPhysics.h"
/**
 * Initializes a train struct with data for train 64
 *
 * @param   physics     The TrainPhysics data to initialize
 *
*/
static inline void trainInit64(TrainPhysics* physics)
{
    trainPhysicsInit(physics);
    trainPhysicsSpeedMap(physics,
                         236758,
                         288793,
                         344585,
                         398728,
                         457684,
                         508239,
                         571681,
                         616784,
                         644170);

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
                         -113256,
                         0,
                         91485,
                         173302,
                         188886,
                         203568,
                         211274,
                         235074,
                         252623,
                         370693);

    trainPhysicsAccelMap(physics, 6,
                         -163256,
                         -114185,
                         0,
                         101612,
                         158438,
                         193204,
                         222605,
                         249664,
                         264825,
                         269419);

    trainPhysicsAccelMap(physics, 7,
                         -183256,
                         -199314,
                         -108385,
                         0,
                         128273,
                         170386,
                         234376,
                         288749,
                         302333,
                         295040);

    trainPhysicsAccelMap(physics, 8,
                         -201428,
                         -257626,
                         -225504,
                         -161635,
                         0,
                         121172,
                         138428,
                         205483,
                         253902,
                         281797);

    trainPhysicsAccelMap(physics, 9,
                         -201428,
                         -257831,
                         -220770,
                         -169676,
                         -74128,
                         0,
                         129768,
                         200827,
                         250341,
                         286660);

    trainPhysicsAccelMap(physics, 10,
                         -294510,
                         -277982,
                         -273701,
                         -261071,
                         -241190,
                         -180000,
                               0,
                           89772,
                          178277,
                          206189);

    trainPhysicsAccelMap(physics, 11,
                         -325196,
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
                         -325209,
                         -318563,
                         -313774,
                         -310389,
                         -290622,
                         -251890,
                         -222537,
                         -146324,
                         0,
                         486187);

    trainPhysicsAccelMap(physics, 13,
                         -343470,
                         -325043,
                         -313774,
                         -306011,
                         -282099,
                         -253769,
                         -253571,
                         -229655,
                         -50180,
                         0);
}

#endif
