/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
extern "C"
{
    #include <SDL2/SDL.h>
}

#include "delta.h"
#include "funcutils.h"

using namespace std;

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

    void Delta::Init(JSON& config)
    {
        /// If the fps cap has a value of <= 0, no capping is applied
        string& data = config["FPS Cap"];
        fpscap = IsFloat(data) ? ToFloat(data) : 0;
        Init();
    }

    void Delta::Init()
    {
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
