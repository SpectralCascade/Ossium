#include <SDL2/SDL.h>

#include "time.h"

namespace ossium
{
    ///
    /// Clock definitions
    ///

    Clock::Clock(Uint32 startTimeMS)
    {
        initialTime = startTimeMS;
        time = 0;
        previousTime = 0;
        scale = 1.0f;
    }

    void Clock::update(float deltaTimeSeconds)
    {
        if (!paused)
        {
            previousTime = time;
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

    void Clock::setPaused(bool pause)
    {
        paused = pause;
    }

    bool Clock::isPaused()
    {
        return paused;
    }

    void Clock::stretch(float scaleFactor)
    {
        scale = scaleFactor;
    }

    float Clock::getScaleFactor()
    {
        return scale;
    }

    void Clock::stepFrames(int frames, float framePeriod)
    {
        float deltaTime = (float)frames * framePeriod * 1000.0f;
        previousTime = time;
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

    Uint32 Clock::getTime()
    {
        return time;
    }

    Uint32 Clock::getInitialTime()
    {
        return initialTime;
    }

    float Clock::getDeltaTime()
    {
        return ((float)time - (float)previousTime) / 1000.0f;
    }

    ///
    /// Timer definitions
    ///

    Timer::Timer(Clock* refClock)
    {
        clock = refClock;
        startTicks = 0;
        pausedTicks = 0;
        started = false;
        paused = false;
    }

    Timer::Timer(const Timer& thisCopy)
    {
        copy(thisCopy);
    }
    Timer Timer::operator=(const Timer& thisCopy)
    {
        copy(thisCopy);
        return *this;
    }

    void Timer::copy(const Timer& thisCopy)
    {
        if (thisCopy.clock != NULL)
        {
            clock = new Clock();
            *clock = *thisCopy.clock;
        }
    }

    void Timer::start()
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

    void Timer::stop()
    {
        started = false;
        paused = false;
        startTicks = 0;
        pausedTicks = 0;
    }

    void Timer::pause()
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

    void Timer::resume()
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

    Uint32 Timer::getTicks()
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

    bool Timer::isStarted()
    {
        return started;
    }
    bool Timer::isPaused()
    {
        return paused;
    }

}
