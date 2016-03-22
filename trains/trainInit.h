#ifndef TRAIN_INIT_H
#define TRAIN_INIT_H

#include "trains/train58.h"
#include "trains/train62.h"
#include "trains/train64.h"
#include "trains/train68.h"
#include "trains/train69.h"

static inline void trainInit(TrainPhysics* physics, const U32 id)
{
    switch (id)
    {
        case 58:
            trainInit58(physics);
            break;

        case 62:
            trainInit62(physics);
            break;

        case 64:
            trainInit64(physics);
            break;

        case 68:
            trainInit68(physics);
            break;

        case 69:
            trainInit69(physics);

        default:
            break;
    }
}

#endif
