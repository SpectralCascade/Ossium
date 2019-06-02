#include <SDL.h>
#include <cmath>

#include "time.h"
#include "basics.h"

using namespace std;

namespace Ossium
{
    ///
    /// Clock definitions
    ///

    Clock::Clock(Uint32 startTimeMS)
    {
        initialTime = startTimeMS;
    }

    bool Clock::Update(float deltaTimeSeconds)
    {
        if (!paused)
        {
            previousTime = time;
            overflow = 0;
            if (scale < 0.0f)
            {
                Uint32 changeInTime = (Uint32)round(((-scale) * deltaTimeSeconds) * 1000.0f);
                if (wrapValue != 0)
                {
                    time = (Uint32)wrap(time, -(int)changeInTime, 0, wrapValue);
                    if (time > previousTime)
                    {
                        overflow = (int)previousTime + (int)wrapValue;
                        return true;
                    }
                }
                else
                {
                    /// Clamping is applied to reversed timers
                    if (changeInTime > time)
                    {
                        time = 0;
                    }
                    else
                    {
                        time -= changeInTime;
                    }
                }
            }
            else
            {
                if (wrapValue != 0)
                {
                    time = (Uint32)wrap(time, (int)round((scale * deltaTimeSeconds) * 1000.0f), 0, wrapValue);
                    if (time < previousTime)
                    {
                        overflow = (int)wrapValue - (int)previousTime;
                        return true;
                    }
                }
                else
                {
                    time += (Uint32)round((scale * deltaTimeSeconds) * 1000.0f);
                }
            }
        }
        return false;
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
            if (wrapValue != 0)
            {
                time = (Uint32)wrap(time, (int)round(deltaTime), 0, wrapValue);
                if (time > previousTime)
                {
                    overflow = (int)previousTime + (int)wrapValue;
                }
            }
            else
            {
                if ((Uint32)round(-deltaTime) > time)
                {
                    time = 0;
                }
                else
                {
                    time -= (Uint32)round(-deltaTime);
                }
            }
        }
        else
        {
            if (wrapValue != 0)
            {
                time = wrap(time, (int)round(deltaTime), 0, wrapValue);
                if (time < previousTime)
                {
                    overflow = (int)wrapValue - (int)previousTime;
                }
            }
            else
            {
                time += (Uint32)round(deltaTime);
            }
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
        if (paused)
        {
            return 0;
        }
        if (overflow != 0)
        {
            return (float)((int)time - overflow) / 1000.0f;
        }
        return (float)((int)time - (int)previousTime) / 1000.0f;
    }

    void Clock::SetTime(Uint32 pos)
    {
        time = pos;
        previousTime = pos;
        overflow = 0;
    }

    void Clock::SetWrap(Uint32 value)
    {
        wrapValue = value;
    }

    Uint32 Clock::GetWrap()
    {
        return wrapValue;
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

    Uint32 GetMS(float seconds)
    {
        return (Uint32)(seconds * 1000.0f);
    }

    float GetSeconds(Uint32 ms)
    {
        return (float)ms / 1000.0f;
    }

}
