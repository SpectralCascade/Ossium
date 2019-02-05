#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "input.h"

namespace Ossium
{

    inline namespace input
    {

        const bool KEY_DOWN = true;
        const bool KEY_UP = false;

        /// Stripped down SDL_Event to pure keyboard data for a specific key
        struct KeyboardInput
        {
            /// Whether the key is pressed or released
            bool state;
            /// Which key is associated with this keyboard event
            SDL_Keycode key;
        };

        /// Handles keyboard input events
        class KeyboardHandler : public InputHandler<KeyboardInput, SDL_Keycode>
        {
        public:
            DECLARE_INPUT_HANDLER(KeyboardHandler);

            bool HandleInput(const SDL_Event& raw);

        };

    }

}

#endif // KEYBOARD_H
