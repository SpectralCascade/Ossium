#include <SDL2/SDL.h>

#include "delta.h"

namespace ossium
{

    Delta::Delta()
    {
        previousTicks = 0;
        deltaTime = 0.0f;
    }

    float Delta::time()
    {
        return deltaTime;
    }

    void Delta::update()
    {
        deltaTime = ((float)SDL_GetTicks() - (float)previousTicks) / 1000.0f;
        previousTicks = SDL_GetTicks();
    }

    void Delta::init()
    {
        previousTicks = SDL_GetTicks();
    }

}
