#include <SDL2/SDL.h>

#include "oss_timeline.h"

OSS_TimeLine::OSS_TimeLine(Uint32 startTimeMS)
{
    initialTime = startTimeMS;
}

/*
float OSS_TimeLine::difference(OSS_TimeLine &timeLine)
{
    return (((float)time) / 1000.0f) - (((float)timeLine.getTime()) / 1000.0f);
}
*/

void OSS_TimeLine::update(float deltaTimeSeconds)
{
    if (!paused)
    {
        Uint32 changeInTime = 0;
        if (scale < 0.0f)
        {
            changeInTime = (Uint32)(((-scale) * deltaTimeSeconds) * 1000.0f);
            /// Check ensures that timeline does not wrap around - no going below 0
            if (changeInTime > time)
            {
                time = 0;
            }
            else
            {
                time -= changeInTime;
            }
        }
        else
        {
            time += (Uint32)((scale * deltaTimeSeconds) * 1000.0f);
        }
    }
}

void OSS_TimeLine::setPaused(bool pause)
{
    paused = pause;
}

void OSS_TimeLine::stretch(float scaleFactor)
{
    scale = scaleFactor;
}

float OSS_TimeLine::getScaleFactor()
{
    return scale;
}

void OSS_TimeLine::stepFrames(int frames, float framePeriod)
{
    float deltaTime = (float)frames * framePeriod * 1000.0f;
    if (deltaTime < 0)
    {
        if ((Uint32)(-deltaTime) > time)
        {
            time = 0;
        }
        else
        {
            time -= (Uint32)(-deltaTime);
        }
    }
    else
    {
        time += (Uint32)deltaTime;
    }
}

Uint32 OSS_TimeLine::getTime()
{
    return time;
}

Uint32 OSS_TimeLine::getInitialTime()
{
    return initialTime;
}
