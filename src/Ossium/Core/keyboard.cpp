/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
#include "keyboard.h"

namespace Ossium
{

    inline namespace Input
    {

        ActionOutcome KeyboardHandler::HandleInput(const SDL_Event& raw)
        {
            if (raw.type == SDL_KEYUP || raw.type == SDL_KEYDOWN)
            {
                /// Extract useful data.
                KeyboardInput data;
                data.state = raw.type == SDL_KEYDOWN ? KEY_DOWN : KEY_UP;
                data.key = raw.key.keysym.sym;
                /// Update associated states, if any.
                UpdateState(data.key, data.state);
                /// Find a bound action linked to this input condition and call it.
                return CallAction(data, data.key);
            }
            return ActionOutcome::Ignore;
        }

    }

}
