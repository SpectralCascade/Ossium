#ifndef FSM_H
#define FSM_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>

#include "serialisation.h"
#include "circularbuffer.h"
#include "ecs.h"

namespace ossium
{

    /// The state interface enables states to be controlled by any class with a state machine
    template<class T>
    class StateInterface
    {
    public:
        virtual void EnterState(T* entity) = 0;
        virtual void ExecuteState(T* entity) = 0;
        virtual void ExitState(T* entity) = 0;

        virtual ~StateInterface()
        {
        }

        string name;

    };

    /// This is a mix-in, rather than a true base class or interface - CRTP is best
    template<class Derived>
    class StateMachine
    {
    public:
        StateMachine()
        {
            history = new CircularBuffer<StateInterface<Derived>*>(50);
            default_state = nullptr;
        }

        /// This shouldn't need to be virtual, as this is a mix-in class - not a true base
        ~StateMachine()
        {
            delete history;
            history = nullptr;
            delete default_state;
            default_state = nullptr;
            for (auto i = states.begin(); i != states.end(); i++)
            {
                delete *i;
                *i = nullptr;
            }
            states.clear();
        }

        /// Sets the default state; this does not change the current state or trigger any state transition logic
        template<class StateType>
        void SetDefaultState(string name = "Default State")
        {
            if (default_state != nullptr)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Default state is being set with name '%s', yet it is already set.", name.c_str());
                delete default_state;
                default_state = nullptr;
            }
            default_state = static_cast<StateInterface<Derived>*>(new StateType());
            default_state->name = name;
        }

        /// Switches to first state found with a matching name
        void SwitchState(string name = "Default State")
        {
            /// First check if the name matches the default state name
            if (default_state != nullptr && name == default_state->name)
            {
                SwitchState(default_state);
                return;
            }
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

        /// Adds a state to the state machine; typically, this should be used on construction or loading of 'Derived'
        template<class StateType>
        void AddState(string name)
        {
            StateInterface<Derived>* newState = static_cast<StateInterface<Derived>*>(new StateType());
            newState->name = name;
            states.push_back(newState);
        }

        StateInterface<Derived>* CurrentState()
        {
            if (history->size() > 0)
            {
                return history->peek_back();
            }
            return default_state;
        }

        /// Returns true and changes state if there is a previous state available,
        /// otherwise returns false and does not change state
        bool SwitchPrevious()
        {
            if (history->size() > 1)
            {
                /// Remove current state and execute it's 'exit' logic
                history->pop_back()->ExitState(DerivedMachine());
                /// Execute the 'enter' logic of the state we've switched back to
                history->peek_back()->EnterState(DerivedMachine());
                return true;
            }
            return false;
        }

    private:
        void SwitchState(StateInterface<Derived>* nextState)
        {
            if (history->size() > 0)
            {
                history->peek_back()->ExitState(DerivedMachine());
            }
            history->push_back(nextState);
            nextState->EnterState(DerivedMachine());
        }

        Derived* DerivedMachine()
        {
            return static_cast<Derived*>(this);
        }

        /// All available states in this state machine
        vector<StateInterface<Derived>*> states;

        /// History of the state machine, as a circular stack. The back-most state is the current state
        CircularBuffer<StateInterface<Derived>*>* history;

        /// The default state of the state machine; when no other states are available, the FSM defaults to this one
        StateInterface<Derived>* default_state;

    };

}

#endif // FSM_H
