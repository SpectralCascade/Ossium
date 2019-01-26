#include "mouse.h"

namespace ossium
{

    REGISTER_INPUT_HANDLER(MouseHandler);

    bool MouseHandler::HandleInput(const SDL_Event& raw)
    {
        MouseInput data;
        data.type = raw.type == SDL_MOUSEMOTION ? MOUSE_MOTION : (raw.type == SDL_MOUSEBUTTONDOWN || raw.type == SDL_MOUSEBUTTONUP ?
                                                                  MOUSE_BUTTON : (raw.type == SDL_MOUSEWHEEL ? MOUSE_WHEEL : MOUSE_UNKNOWN));
        if (data.type != MOUSE_UNKNOWN)
        {
            data.button = raw.button.button;
            data.direction = raw.wheel.direction;
            data.clicks = raw.button.clicks;
            data.xrel = raw.motion.xrel;
            data.xrel = raw.motion.yrel;
            if (data.type == MOUSE_MOTION)
            {
                data.x = raw.motion.x;
                data.y = raw.motion.y;
                data.state = raw.motion.state;
            }
            else if (data.type == MOUSE_BUTTON)
            {
                data.state = raw.button.state == SDL_PRESSED ? MOUSE_PRESSED : MOUSE_RELEASED;
            }
            else if (data.type == MOUSE_WHEEL)
            {
                data.x = raw.wheel.x;
                data.y = raw.wheel.y;
                data.state = raw.button.state == SDL_PRESSED ? MOUSE_PRESSED : MOUSE_RELEASED;
            }

            return CallAction(data, data.type);
        }
        return false;
    }

}
