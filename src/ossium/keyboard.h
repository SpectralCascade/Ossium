#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "input.h"

namespace ossium
{

    const bool KEY_DOWN = true;
    const bool KEY_UP = false;

    /// Stripped down SDL_Event to pure keyboard data for a specific key
    struct KeyboardInput
    {
        bool state;
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

#endif // KEYBOARD_H
