#ifndef TRAIN_INIT_H
#define TRAIN_INIT_H

#include "trains/train62.h"
#include "trains/train68.h"

static inline void trainInit(TrainPhysics* physics, const U32 id)
{
    switch (id)
    {
        case 62:
            trainInit62(physics);
            break;

        case 68:
            trainInit68(physics);
            break;

        default:
            break;
    }
}

#endif