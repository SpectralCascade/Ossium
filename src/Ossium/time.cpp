#include <SDL.h>

#include "time.h"

namespace Ossium
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

    void Clock::Update(float deltaTimeSeconds)
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

    void Clock::SetPaused(bool pause)
    {
        paused = pause;
    }

    bool Clock::IsPaused()
    {
        return paused;
    }

    void Clock::Stretch(float scaleFactor)
    {
        scale = scaleFactor;
    }

    float Clock::GetScaleFactor()
    {
        return scale;
    }

    void Clock::StepFrames(int frames, float framePeriod)
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

    Uint32 Clock::GetTime()
    {
        return time;
    }

    Uint32 Clock::GetInitialTime()
    {
        return initialTime;
    }

    float Clock::GetDeltaTime()
    {
        return ((float)time - (float)previousTime) / 1000.0f;
    }

    ///
    /// Timer definitions
    ///

    Timer::Timer(Clock* refClock)
    {
        clock = refClock;
    }

    Timer::Timer(const Timer& source)
    {
        paused = source.paused;
        started = source.started;
        startTicks = source.startTicks;
        pausedTicks = source.pausedTicks;
        if (source.clock != nullptr)
        {
            clock = new Clock(*source.clock);
        }
    }
    Timer& Timer::operator=(const Timer& source)
    {
        *this = Timer(source);
        return *this;
    }

    void Timer::Start()
    {
        started = true;
        paused = false;
        pausedTicks = 0;
        if (clock != nullptr)
        {
            startTicks = clock->GetTime();
        }
        else
        {
            startTicks = SDL_GetTicks();
        }
    }

    void Timer::Stop()
    {
        started = false;
        paused = false;
        startTicks = 0;
        pausedTicks = 0;
    }

    void Timer::Pause()
    {
        if (started && !paused)
        {
            paused = true;
            if (clock != nullptr)
            {
                pausedTicks = clock->GetTime() - startTicks;
            }
            else
            {
                pausedTicks = SDL_GetTicks() - startTicks;
            }
            startTicks = 0;
        }
    }

    void Timer::Resume()
    {
        if (started && paused)
        {
            paused = false;
            if (clock != nullptr)
            {
                startTicks = clock->GetTime() - pausedTicks;
            }
            else
            {
                startTicks = SDL_GetTicks() - pausedTicks;
            }
            pausedTicks = 0;
        }
    }

    Uint32 Timer::GetTicks()
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
                if (clock != nullptr)
                {
                    time = clock->GetTime() - startTicks;
                }
                else
                {
                    time = SDL_GetTicks() - startTicks;
                }
            }
        }
        return time;
    }

    bool Timer::IsStarted()
    {
        return started;
    }
    bool Timer::IsPaused()
    {
        return paused;
    }

}
