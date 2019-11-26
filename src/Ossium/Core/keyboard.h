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
