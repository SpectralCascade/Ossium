#include "input.h"

namespace Ossium
{

    inline namespace Input
    {
/*
        namespace Internals
        {

            InputHandlerType TypeRegistry<InputHandlerType>::nextTypeIdent = 0;

        }*/

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

        void InputController::Update()
        {
            SDL_Event current;
            bool claimedFinal = false;
            while (SDL_PollEvent(&current) != 0)
            {
                for (auto i = contexts.begin(); i != contexts.end(); i++)
                {
                    ActionOutcome contextOutcome = (*i).second->HandleInput(current);
                    if (contextOutcome == ActionOutcome::ClaimFinal)
                    {
                        claimedFinal = true;
                        break;
                    }
                    else if (contextOutcome == ActionOutcome::ClaimGlobal)
                    {
                        /// Prevent other contexts from handling this specific input event as it has been claimed globally.
                        break;
                    }
                }
                if (claimedFinal)
                {
                    /// Get rid of all input events to prevent any more input event handling taking place.
                    SDL_FlushEvents(0, 0xFFFFFFFF);
                    break;
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

        ActionOutcome InputController::HandleEvent(const SDL_Event& raw)
        {
            for (auto i = contexts.begin(); i != contexts.end(); i++)
            {
                ActionOutcome outcome = (*i).second->HandleInput(raw);
                if (outcome == ActionOutcome::ClaimGlobal || outcome == ActionOutcome::ClaimFinal)
                {
                    return outcome;
                }
            }
            return ActionOutcome::Ignore;
        }

        void InputController::Clear()
        {
            contexts.clear();
        }

    }

}
