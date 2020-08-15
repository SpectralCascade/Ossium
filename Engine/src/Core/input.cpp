/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
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
#include "input.h"

using namespace std;

namespace Ossium
{

    InputContext* BaseInputHandler::GetContext()
    {
        return context;
    }

    string GetActionOutcomeName(ActionOutcome outcome)
    {
        switch (outcome)
        {
            case ActionOutcome::ClaimContext:
            {
                return "ClaimContext";
            }
            case ActionOutcome::ClaimGlobal:
            {
                return "ClaimGlobal";
            }
            case ActionOutcome::ClaimFinal:
            {
                return "ClaimFinal";
            }
            case ActionOutcome::Ignore:
            {
                return "Ignore";
            }
        }
        return "Ignore";
    }

    InputController::InputController()
    {
    }

    InputController::~InputController()
    {
        Clear();
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

    InputContext* InputController::GetContext(string name)
    {
        auto itr = contexts.find(name);
        if (itr != contexts.end())
        {
            return itr->second;
        }
        return nullptr;
    }

    bool InputController::HandleEvent(const SDL_Event& raw)
    {
        /// Handle event across all contexts
        for (auto i = contexts.begin(); i != contexts.end(); i++)
        {
            ActionOutcome contextOutcome = (*i).second->HandleInput(raw);
            if (contextOutcome == ActionOutcome::ClaimFinal)
            {
                /// All remaining input events should be discarded regardless of type or context.
                return true;
            }
            else if (contextOutcome == ActionOutcome::ClaimGlobal)
            {
                /// Prevent other contexts from handling this specific input event as it has been claimed globally.
                break;
            }
        }
        return false;
    }

    void InputController::Clear()
    {
        contexts.clear();
    }

}
