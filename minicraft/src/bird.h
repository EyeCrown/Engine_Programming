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
        float x = fmod(randf(), 2.0f) - 0.5f;
        float y = fmod(randf(), 2.0f) - 0.5f;
        float z = fmod(randf(), 2.0f) - 0.5f;
        Direction += YVec3f(x * elapsed, y * elapsed, z * elapsed);
        Direction.normalize();
    }
    
};
