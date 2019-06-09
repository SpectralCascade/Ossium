#include <SDL.h>

#include "delta.h"

namespace Ossium
{

    Delta::Delta()
    {
        previousTicks = 0;
        deltaTime = 0;
        fpscap = 0;
    }

    float Delta::Time()
    {
        return deltaTime;
    }

    void Delta::Update()
    {
        deltaTime = ((float)SDL_GetTicks() - (float)previousTicks) / 1000.0f;
        if (fpscap > 0)
        {
            float maxDelta = 1.0f / fpscap;
            if (deltaTime < maxDelta)
            {
                SDL_Delay((Uint32)((maxDelta - deltaTime) * 1000.0f));
            }
        }
        previousTicks = SDL_GetTicks();
    }

    void Delta::Init(Config& config)
    {
        /// If the fps cap has a value of <= 0, no capping is applied
        fpscap = config.fpscap;
        previousTicks = SDL_GetTicks();
    }

    void Delta::Reset()
    {
        previousTicks = SDL_GetTicks();
    }

    inline namespace Global
    {

        Delta delta;

    }

}
