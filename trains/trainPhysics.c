#include "trains/trainPhysics.h"

void trainPhysicsInit(TrainPhysics* physics)
{
    physics->velocity = 0;
    physics->acceleration = 0;
    physics->targetVelocity = 0;
    physics->speed = 0;
    physics->targetSpeed = 0;
}

void trainPhysicsSpeedMap(TrainPhysics* physics, 
                          const S32 v3, const S32 v4, const S32 v5,
                          const S32 v6, const S32 v7, const S32 v8,
                          const S32 v9, const S32 v10, const S32 v11,
                          const S32 v12, const S32 v13, const S32 v14)
{
    physics->speedMap[ 0] = 0;
    physics->speedMap[ 1] = 0;
    physics->speedMap[ 2] = 0;

    physics->speedMap[ 3] = v3;
    physics->speedMap[ 4] = v4;
    physics->speedMap[ 5] = v5;
    physics->speedMap[ 6] = v6;
    physics->speedMap[ 7] = v7;
    physics->speedMap[ 8] = v8;
    physics->speedMap[ 9] = v9;
    physics->speedMap[10] = v10;
    physics->speedMap[11] = v11;
    physics->speedMap[12] = v12;
    physics->speedMap[13] = v13;
    physics->speedMap[14] = v14;
}

void trainPhysicsAccelMap(TrainPhysics* physics,
                          const U8 speed,
                          const S32 m,
                          const S32 b)
{
    trainAccelerationInit(&(physics->accelMap[speed]), m, b);
}

void trainPhysicsStep(TrainPhysics* physics, const U32 delta)
{
    if (physics->acceleration != 0)
    {
        U32 ktick = ((delta / 100) + 5)/10;

        S32 a = physics->acceleration / 10;
        a += 5;
        a /= 10;

        physics->velocity += (a * ktick);

        if (physics->velocity >= physics->targetVelocity)
        {
            physics->acceleration = 0;
        }
    }
}

void trainPhysicsSetSpeed(TrainPhysics* physics, const U8 speed)
{
    physics->targetVelocity = physics->speedMap[speed];
    physics->targetSpeed = speed;

    physics->acceleration =
        trainAccelerationGet(&(physics->accelMap[physics->speed]),
                             speed);
}

void trainPhysicsReport(TrainPhysics* physics,
                        const S32 dist,
                        const U32 ticks)
{
}

S32 trainPhysicsGetVelocity(TrainPhysics* physics)
{
    return physics->velocity;
}

U32 trainPhysicsGetTime(TrainPhysics* physics, const S32 dx)
{
    U32 ticks = 0;

    if (physics->acceleration == 0)
    {
        ticks = (((dx * 1000 * 10000) / physics->velocity)+5)/10;
    }
    else
    {
        // vf = vi + a*t
        // t = (vf-vi)/a
        U32 accelTime = physics->targetVelocity - physics->velocity;
        accelTime += 5;
        accelTime /= 1000;
        accelTime /= physics->acceleration;

        // 0.5at^2 + v*t = x
        U32 vDist = physics->velocity * accelTime;
        vDist /= 100000;
        vDist += 5;
        vDist /= 10;

        U32 aDist = accelTime * accelTime;
        aDist /= 2000;
        aDist *= physics->acceleration;
        aDist /= 100000;
        aDist += 5;
        aDist /= 10;

        U32 totalDist = aDist + vDist;

        // x <= dx
        if (totalDist <= dx)
        {
            ticks = accelTime;
            ticks += (dx-totalDist * 1000 * 1000) / physics->targetVelocity;
        }
        // x > dx
        else
        {
            return 0;
        }
    }

    return ticks;
}
