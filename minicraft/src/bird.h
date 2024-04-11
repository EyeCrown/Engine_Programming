#pragma once
#include "engine/utils/types_3d.h"

class MBird
{
public:

    YVec3f Position;
    YVec3f Direction;

    MBird(YVec3f initialPosition, YVec3f initialDirection)
    {
        Position = initialPosition;
        Direction = initialDirection;
    }

    void Update(float elapsed)
    {
        UpdatePosition(elapsed);
        UpdateDirection(elapsed);
        //YLog::log(YLog::ENGINE_INFO, ("Bird position: " + Position.toStr() + "\nBird direction: " + Direction.toStr()).c_str());
    }

    void UpdatePosition(float elapsed)
    {
        Position += Direction * elapsed;
    }

    void UpdateDirection(float elapsed)
    {
        Direction += YVec3f(randf() * elapsed, randf() * elapsed, randf() * elapsed);
        Direction.normalize();
    }
    
};
