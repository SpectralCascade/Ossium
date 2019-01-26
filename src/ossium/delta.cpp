#include <SDL.h>

#include "delta.h"

namespace ossium
{

    Delta::Delta()
    {
        previousTicks = 0;
        deltaTime = 0;
        fpscap = 0;
    }

    float Delta::time()
    {
        return deltaTime;
    }

    void Delta::update()
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

    void Delta::init(Config& config)
    {
        /// If the fps cap has a value of <= 0, no capping is applied
        fpscap = config.fpscap;
        previousTicks = SDL_GetTicks();
    }

    void Delta::reset()
    {
        previousTicks = SDL_GetTicks();
    }

}
