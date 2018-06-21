#include <SDL2/SDL.h>

#include "oss_timer.h"

OSS_Timer::OSS_Timer()
{
    startTicks = 0;
    pausedTicks = 0;
    started = false;
    paused = false;
}

void OSS_Timer::start()
{
    started = true;
    paused = false;
    startTicks = SDL_GetTicks();
    pausedTicks = 0;
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
        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
}

void OSS_Timer::resume()
{
    if (started && paused)
    {
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
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
            time = SDL_GetTicks() - startTicks;
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
