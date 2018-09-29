#include <SDL2/SDL.h>

#include "oss_time.h"

///
/// OSS_Clock definitions
///

OSS_Clock::OSS_Clock(Uint32 startTimeMS)
{
    initialTime = startTimeMS;
}

void OSS_Clock::update(float deltaTimeSeconds)
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

void OSS_Clock::setPaused(bool pause)
{
    paused = pause;
}

bool OSS_Clock::isPaused()
{
    return paused;
}

void OSS_Clock::stretch(float scaleFactor)
{
    scale = scaleFactor;
}

float OSS_Clock::getScaleFactor()
{
    return scale;
}

void OSS_Clock::stepFrames(int frames, float framePeriod)
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

Uint32 OSS_Clock::getTime()
{
    return time;
}

Uint32 OSS_Clock::getInitialTime()
{
    return initialTime;
}

///
/// OSS_Timer definitions
///

OSS_Timer::OSS_Timer(OSS_Clock* refClock)
{
    clock = refClock;
    startTicks = 0;
    pausedTicks = 0;
    started = false;
    paused = false;
}

void OSS_Timer::start()
{
    started = true;
    paused = false;
    pausedTicks = 0;
    if (clock != NULL)
    {
        startTicks = clock->getTime();
    }
    else
    {
        startTicks = SDL_GetTicks();
    }
}

void OSS_Timer::stop()
{
    started = false;
    paused = false;
    startTicks = 0;
    pausedTicks = 0;
}

void OSS_Timer::pause()
{
    if (started && !paused)
    {
        paused = true;
        if (clock != NULL)
        {
            pausedTicks = clock->getTime() - startTicks;
        }
        else
        {
            pausedTicks = SDL_GetTicks() - startTicks;
        }
        startTicks = 0;
    }
}

void OSS_Timer::resume()
{
    if (started && paused)
    {
        paused = false;
        if (clock != NULL)
        {
            startTicks = clock->getTime() - pausedTicks;
        }
        else
        {
            startTicks = SDL_GetTicks() - pausedTicks;
        }
        pausedTicks = 0;
    }
}

Uint32 OSS_Timer::getTicks()
{
    Uint32 time = 0;
    if (started)
    {
        if (paused)
        {
            time = pausedTicks;
        }
        else
        {
            if (clock != NULL)
            {
                time = clock->getTime() - startTicks;
            }
            else
            {
                time = SDL_GetTicks() - startTicks;
            }
        }
    }
    return time;
}

bool OSS_Timer::isStarted()
{
    return started;
}
bool OSS_Timer::isPaused()
{
    return paused;
}
