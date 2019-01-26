#include "keyboard.h"

namespace ossium
{

    REGISTER_INPUT_HANDLER(KeyboardHandler);

    bool KeyboardHandler::HandleInput(const SDL_Event& raw)
    {
        if (raw.type == SDL_KEYUP || raw.type == SDL_KEYDOWN)
        {
            /// Extract useful data
            KeyboardInput data;
            data.state = raw.type == SDL_KEYDOWN ? KEY_DOWN : KEY_UP;
            data.key = raw.key.keysym.sym;
            /// Find a bound action linked to this input condition
            /// Note that the equality operator overload means that the key state is ignored in the lookup
            auto actionItr = _input_map.find(data.key);
            if (actionItr != _input_map.end())
            {
                /// Call the action and pass in the input data
                (*actionItr).second(data);
                return true;
            }
        }
        return false;
    }

}
