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

        string name;

    };

    /// This is a mix-in, rather than a true base class or interface
    template<class Derived>
    class StateMachine
    {
    public:
        StateMachine()
        {
            history = nullptr;
        }

        /// This doesn't need to be virtual, as this is a mix-in class
        ~StateMachine()
        {
            delete history;
            history = nullptr;
            states.clear();
        }

        void SwitchState(StateInterface<Derived>* nextState)
        {
            #ifdef DEBUG
            SDL_assert(nextState != nullptr);
            #endif // DEBUG
            history->peek_back()->ExitState(DerivedMachine());
            history->push_back(nextState);
            nextState->EnterState(DerivedMachine());
        }

        void SwitchState(string name)
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

        /// Adds a state to the state machine; typically, this should be used on construction or loading of 'Derived'
        void AddState(StateInterface<Derived>* newState)
        {
            states.push_back(newState);
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
        Derived* DerivedMachine()
        {
            return static_cast<Derived*>(this);
        }

        /// All available states in this state machine
        vector<StateInterface<Derived>*> states;

        /// History of the state machine, as a circular stack
        CircularBuffer<StateInterface<Derived>*>* history;

    };

}

#endif // FSM_H
