#include "input.h"

namespace Ossium
{

    inline namespace input
    {
/*
        namespace internals
        {

            InputHandlerType TypeRegistry<InputHandlerType>::nextTypeIdent = 0;

        }*/

        InputController::InputController()
        {
        }

        InputController::~InputController()
        {
            Clear();
        }

        void InputController::Update()
        {
            SDL_Event current;
            while (SDL_PollEvent(&current) != 0)
            {
                for (auto i = contexts.begin(); i != contexts.end(); i++)
                {
                    (*i).second->HandleInput(current);
                }
            }
        }

        void InputController::AddContext(string name, InputContext* context)
        {
            contexts[name] = context;
        }

        void InputController::RemoveContext(string name)
        {
            auto itr = contexts.find(name);
            if (itr != contexts.end())
            {
                contexts.erase(itr);
            }
        }

        bool InputController::HandleEvent(const SDL_Event& raw)
        {
            for (auto i = contexts.begin(); i != contexts.end(); i++)
            {
                if ((*i).second->HandleInput(raw))
                {
                    return true;
                }
            }
            return false;
        }

        void InputController::Clear()
        {
            contexts.clear();
        }

    }

}
