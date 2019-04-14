#ifndef FSM_H
#define FSM_H

#include <vector>
#include <string>
#include <SDL.h>

#include "serialisation.h"
#include "circularbuffer.h"
#include "ecs.h"

namespace Ossium
{

    /// The state interface enables states to be controlled by any class with a state machine
    template<class StateMachineType>
    class StateInterface
    {
    public:
        StateInterface(StateMachineType* machine)
        {
            stateMachine = machine;
        }

        virtual void EnterState() = 0;
        virtual void ExecuteState() = 0;
        virtual void ExitState() = 0;

        virtual ~StateInterface()
        {
        }

        string name;

    protected:
        StateMachineType* stateMachine;

    };

    /// This is a mix-in, rather than a true base class or interface - CRTP is best
    template<class Derived>
    class StateMachine
    {
    public:
        StateMachine()
        {
            history = new CircularBuffer<StateInterface<Derived>*>(50);
            pre_current_state = nullptr;
            post_current_state = nullptr;
            current_state_index = -1;
        }

        /// This shouldn't need to be virtual, as this is a mix-in class - not a true base
        ~StateMachine()
        {
            delete history;
            history = nullptr;
            if (pre_current_state != nullptr)
            {
                delete pre_current_state;
                pre_current_state = nullptr;
            }
            if (post_current_state != nullptr)
            {
                delete post_current_state;
                post_current_state = nullptr;
            }
            for (auto i = states.begin(); i != states.end(); i++)
            {
                delete *i;
                *i = nullptr;
            }
            states.clear();
        }

        void UpdateFSM()
        {
            if (pre_current_state != nullptr)
            {
                pre_current_state->ExecuteState();
            }
            if (current_state_index >= 0)
            {
                (*history)[current_state_index]->ExecuteState();
            }
            if (post_current_state != nullptr)
            {
                post_current_state->ExecuteState();
            }
        }

        /// Sets the pre-current state; triggers any pre-current transition logic
        template<class StateType>
        void SetPreState()
        {
            SetGlobalState<StateType>(pre_current_state);
            if (pre_current_state->name.length() == 0)
            {
                pre_current_state->name = "PRE_CURRENT_STATE";
            }
        }

        /// Sets the post-current state; triggers any post-current transition logic
        template<class StateType>
        void SetPostState()
        {
            SetGlobalState<StateType>(post_current_state);
            if (post_current_state->name.length() == 0)
            {
                post_current_state->name = "POST_CURRENT_STATE";
            }
        }

        /// Switches to first state found with a matching name
        void SwitchState(string name = "Default State")
        {
            for (auto i = states.begin(); i != states.end(); i++)
            {
                if ((*i)->name == name)
                {
                    SwitchState(*i);
                    return;
                }
            }
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Could not find state '%s' in StateMachine.", name.c_str());
        }

        /// Adds a state to the state machine; typically,
        /// this should be used on construction or loading of DerivedMachine()
        template<class StateType>
        void AddState(string name = "")
        {
            StateInterface<Derived>* newState = static_cast<StateInterface<Derived>*>(new StateType(DerivedMachine()));
            if (name.length() > 0)
            {
                newState->name = name;
            }
            states.push_back(newState);
        }

        /// Warning: Returns nullptr if there is no current state
        StateInterface<Derived>* CurrentState()
        {
            if (current_state_index >= 0)
            {
                return (*history)[current_state_index];
            }
            return nullptr;
        }

        /// Returns true and changes state if there is a previous state available,
        /// otherwise returns false and does not change state
        bool SwitchPrevious()
        {
            if (current_state_index > 0)
            {
                (*history)[current_state_index]->ExitState();
                current_state_index--;
                (*history)[current_state_index]->EnterState();
                /// Execute the 'enter' logic of the state we've switched back to
                history->peek_back()->EnterState();
                return true;
            }
            else if (current_state_index == 0)
            {
                (*history)[current_state_index]->ExitState();
                current_state_index = -1;
            }
            return false;
        }

        bool SwitchNext()
        {
            if (current_state_index >= 0 && current_state_index < history->size() - 1)
            {
                (*history)[current_state_index]->ExitState();
                current_state_index++;
                (*history)[current_state_index]->EnterState();
                return true;
            }
            else if (current_state_index < 0 && history->size() > 0)
            {
                current_state_index = 0;
                (*history)[current_state_index]->EnterState();
                return true;
            }
            return false;
        }

    private:
        /// TODO: what's going on with global, pre and post states???
        template<class StateType>
        void SetGlobalState(StateInterface<Derived>* &global_state)
        {
            if (global_state != nullptr)
            {
                global_state->ExitState();
                delete global_state;
                global_state = nullptr;
            }
            global_state = static_cast<StateInterface<Derived>*>(new StateType(DerivedMachine()));
        }

        void SwitchState(StateInterface<Derived>* nextState)
        {
            if (current_state_index >= 0)
            {
                (*history)[current_state_index]->ExitState();
            }
            current_state_index++;
            if (current_state_index < history->size() - 1)
            {
                history->drop_back_to(current_state_index - 1);
            }
            history->push_back(nextState);
            nextState->EnterState();
        }

        Derived* DerivedMachine()
        {
            return static_cast<Derived*>(this);
        }

        /// All available states in this state machine
        vector<StateInterface<Derived>*> states;

        /// History of the state machine, as a circular stack. The back-most state is the current state
        CircularBuffer<StateInterface<Derived>*>* history;

        /// The current state index (in state history) relative to the front item
        int current_state_index;

        /// The pre and post states execute before and after the current state respectively
        StateInterface<Derived>* pre_current_state;
        StateInterface<Derived>* post_current_state;

    };

}

#endif // FSM_H
