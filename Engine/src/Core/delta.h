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
#pragma once

#ifndef DELTA_H
#define DELTA_H

#include "jsondata.h"

namespace Ossium
{

    class OSSIUM_EDL Delta
    {
    public:
        Delta();

        /// Get the change in time between calls to Update()
        float Time();

        /// Updates values in the class each time it is called
        void Update();

        /// Applies FPS capping configuration and initialises previousTicks to current ticks
        void Init(JSON& config);
        void Init();

        /// Resets previousTicks to current ticks
        void Reset();

    private:
        Uint32 previousTicks;

        float deltaTime;

        float fpscap;

    };

}

#endif // DELTA_H
