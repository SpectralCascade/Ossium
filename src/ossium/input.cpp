#include "input.h"

namespace ossium
{

    namespace inputsys
    {

        InputHandlerType HandlerRegistry::nextTypeIdent = 0;

    }

    Input::Input()
    {
    }

    Input::~Input()
    {
        Clear();
    }

    void Input::Update()
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

    void Input::AddContext(string name, InputContext* context)
    {
        contexts[name] = context;
    }

    void Input::RemoveContext(string name)
    {
        auto itr = contexts.find(name);
        if (itr != contexts.end())
        {
            contexts.erase(itr);
        }
    }

    bool Input::HandleEvent(const SDL_Event& raw)
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

    void Input::Clear()
    {
        contexts.clear();
    }

}
