#include "input.h"

namespace Ossium
{

    inline namespace Input
    {

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
            Logger::EngineLog().Warning("Failed to retrieve context {0} from input controller.", name);
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

}
