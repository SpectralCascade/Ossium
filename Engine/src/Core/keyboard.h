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
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "input.h"

namespace Ossium
{

    inline namespace Input
    {

        const bool KEY_DOWN = true;
        const bool KEY_UP = false;

        /// Stripped down SDL_Event to pure keyboard data for a specific key
        struct OSSIUM_EDL KeyboardInput
        {
            /// Whether the key is pressed or released
            bool state;
            /// Which key is associated with this keyboard event
            SDL_Keycode key;
        };

        /// Handles keyboard input events
        class OSSIUM_EDL KeyboardHandler : public InputHandler<KeyboardHandler, KeyboardInput, SDL_Keycode>
        {
        public:
            ActionOutcome HandleInput(const SDL_Event& raw);

        };

    }

}

#endif // KEYBOARD_H
