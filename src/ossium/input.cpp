#include "input.h"

namespace ossium
{

    namespace inputsys
    {

        InputHandlerType HandlerRegistry::nextTypeIdent = 0;

    }

    void Input::Update()
    {
        SDL_Event current;
        while (SDL_PollEvent(&current) != 0)
        {
            for (auto i = contexts.begin(); i != contexts.end(); i++)
            {
                (*i).HandleInput(current);
            }
        }
        /*vector<InputData> inputs;
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            InputData data;
            data.raw = event.type;
            data.type = INPUT_UNKNOWN;
            data.value = 0;
            data.state = 0;
            data.range = {0, 0};

            /// Massage the data into our abstract input data structure
            if (event.key.keysym.sym != SDLK_UNKNOWN)
            {
                data.type = INPUT_KEYBOARD;
                data.key = event.key.keysym.sym;
            }
            else if (event.type >= SDL_CONTROLLERAXISMOTION && event.type <= SDL_CONTROLLERDEVICEREMAPPED)
            {
                data.type = INPUT_CONTROLLER;
                if (event.caxis.axis == 0)
                {
                    data.range.x = (float)event.caxis.value;
                }
                else
                {
                    data.range.y = (float)event.caxis.value;
                }
                data.button = event.cbutton.button;
                data.state = event.cbutton.state;
            }
            else if (event.type >= SDL_MOUSEMOTION && event.type <= SDL_MOUSEWHEEL)
            {
                data.type == INPUT_MOUSE;
                data.button = event.button.button;
                data.state = event.button.state;
                data.range.x = (float)event.motion.x;
                data.range.y = (float)event.motion.y;
            }
            else if (event.type >= SDL_JOYAXISMOTION && event.type <= SDL_JOYDEVICEREMOVED)
            {
                data.type = INPUT_JOYSTICK;
                if (data.raw == SDL_JOYBUTTONDOWN || data.raw == SDL_JOYBUTTONUP)
                {
                    data.button = event.jbutton.button;
                    data.state = event.jbutton.state;
                }
                else if (data.raw == SDL_JOYAXISMOTION)
                {
                    if (event.jaxis.axis == 0)
                    {
                        data.range.x = (float)event.jaxis.value;
                    }
                    else
                    {
                        data.range.y = (float)event.jaxis.value;
                    }
                }
                else if (data.raw == SDL_JOYBALLMOTION)
                {
                    data.value = (float)event.jball.ball;
                    data.range.x = (float)event.jball.xrel;
                    data.range.y = (float)event.jaxis.yrel;
                }
                else if (data.raw == SDL_JOYHATMOTION)
                {
                    data.value = (float)event.jhat.hat;
                    data.state = (float)event.jhat.value;
                }
            }
            else if (event.type >= SDL_FINGERDOWN && event.type <= SDL_FINGERMOTION)
            {
                data.type = INPUT_TOUCH;
                data.finger = event.tfinger.fingerId;
                data.value = event.tfinger.pressure;
                data.range.x = event.tfinger.x;
                data.range.y = event.tfinger.y;
            }
            else if (event.type == SDL_DOLLARGESTURE)
            {
                data.type = INPUT_GESTURE;
                data.range.x = event.dgesture.x;
                data.range.y = event.dgesture.y;
                data.value = event.dgesture.error;
                data.numberOf = event.dgesture.numFingers;
            }
            else if (event.type == SDL_MULTIGESTURE)
            {
                data.type = INPUT_MULTIGESTURE;
                data.numberOf = event.mgesture.numFingers;
                data.delta = event.mgesture.dDist;
                data.value = event.mgesture.dTheta;
                data.range.x = event.mgesture.x;
                data.range.y = event.mgesture.y;
            }
            else if (event.type == SDL_SENSORUPDATE)
            {
                data.type = INPUT_SENSOR;
                data.value = event.sensor.data;
            }
            /// Put the input data in the list
            inputs.push_back(data);
        }

        for (auto i = contexts.begin(); i != contexts.end(); i++)
        {
            if ((*i).IsActive())
            {
                for (auto j = (*i).inputMap.begin(); j != (*i).inputMap.end(); j++)
                {
                }
            }
        }*/
    }

}
