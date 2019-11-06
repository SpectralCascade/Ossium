#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <SDL.h>

#include "basics.h"
#include "helpermacros.h"

using namespace std;

namespace Ossium
{

    inline namespace Input
    {

        /// This enumeration is used to determine how the input system should behave when calling actions during input handling.
        /// Return one of these values from an action to indicate whether the system should continue handling a particular input.
        /// Note that these outcomes also affect states as states will not be updated after claiming!
        enum ActionOutcome
        {
            /// Other actions within a context will continue to be evaluated regardless.
            /// If in doubt, return this from your actions.
            Ignore = 0,
            /// Other actions within the local context will NO LONGER be evaluated,
            /// but actions and states associated with the input will still be evaluated in other contexts.
            ClaimContext = 1,
            /// Other actions associated with the input will NOT be evaluated again this frame, regardless of context.
            /// It's recommended that you avoid returning this in your actions as it prevents associated states updating in other contexts,
            /// though in some cases it might be useful (e.g. when the player sets custom input bindings).
            ClaimGlobal = 2,
            /// NO other actions will be evaluated this frame, EVEN if they are not associated with the input!
            /// Don't return this in your actions unless you want to stop handling all inputs for the current Update().
            ClaimFinal = 3
        };

        /// Returns the name of an action outcome value
        string GetActionOutcomeName(ActionOutcome outcome);

        typedef Uint32 InputHandlerType;

        /// Declares an input handler type
        /// Add this to the end of any class you wish to register as a component
        #define DECLARE_INPUT_HANDLER(TYPE) public: static Ossium::typesys::TypeRegistry<InputHandlerType> __input_type_entry_

        /// Adds the input handler type to the registry by static instantiation
        /// Add this to the class definition of an input handler that uses DECLARE_INPUT_HANDLER
        #define REGISTER_INPUT_HANDLER(TYPE) Ossium::typesys::TypeRegistry<InputHandlerType> TYPE::__input_type_entry_

        /// Constant return type id for a specified input handler type
        template<class T>
        InputHandlerType GetInputHandlerType()
        {
            return T::__input_type_entry_.GetType();
        }

        /// Interface for accessing InputHandler
        class BaseInputHandler
        {
        public:
            virtual ActionOutcome HandleInput(const SDL_Event& raw) = 0;
            virtual ~BaseInputHandler()
            {
            }

        };

        /// An abstract interface for handling a specific type of input; template takes a specialised structure that contains only relevant information
        /// stripped from an SDL_Event, and an ident type used to map an input directly to an action (e.g. SDL_Keycode)
        template<class InputData, class InputIdent>
        class InputHandler : public BaseInputHandler
        {
        public:
            /// Function pointer for actions based on this input data
            typedef function<ActionOutcome(const InputData&)> InputAction;

            virtual ~InputHandler()
            {
                Clear();
            }

            /// Takes raw input data and strips away the irrelevant stuff, then checks if it matches anything in the actions table
            /// Returns true if the raw event data should be discarded after handling
            virtual ActionOutcome HandleInput(const SDL_Event& raw) = 0;

            /// Adds a state tracker to the input handler
            void AddState(string name)
            {
                InputIdent junk = InputIdent();
                _input_state_bindings[name] = junk;
            }

            /// Simplifies adding states.
            void AddState(string name, InputIdent binding)
            {
                AddState(name);
                BindState(name, binding);
            }

            bool GetState(string name)
            {
                auto itr = _input_state_bindings.find(name);
                if (itr != _input_state_bindings.end())
                {
                    return _state_map[itr->second];
                }
                return false;
            }

            /// Removes the specified state. Returns false if the state does not exist.
            void RemoveState(string name)
            {
                auto binding = _input_state_bindings.find(name);
                if (binding != _input_state_bindings.end())
                {
                    auto directbind = _state_map.find(binding->second);
                    if (directbind != _state_map.end())
                    {
                        _state_map.erase(directbind);
                    }
                    _input_state_bindings.erase(binding);
                }
            }

            /// Binds a state to a specific input
            void BindState(string name, InputIdent stateIdent)
            {
                RemoveState(name);
                _input_state_bindings[name] = stateIdent;
                _state_map[stateIdent] = false;
            }

            /// Returns the input ident bound to the specified input
            const InputIdent* GetStateBind(string name)
            {
                auto itr = _input_state_bindings.find(name);
                if (itr != _input_state_bindings.end())
                {
                    return &((*itr).second);
                }
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Could not find binding value for state '%s'.", name.c_str());
                return nullptr;
            }

            /// This method adds a callback for a specific game action
            void AddAction(string name, InputAction action)
            {
                _action_bindings[name] = action;
            }

            /// Simplifies adding actions.
            void AddAction(string name, InputAction action, InputIdent binding)
            {
                AddAction(name, action);
                BindAction(name, binding);
            }

            /// Removes the specified action. Returns false if the action does not exist.
            void RemoveAction(string name)
            {
                auto actionbind = _action_bindings.find(name);
                if (actionbind != _action_bindings.end())
                {
                    _action_bindings.erase(actionbind);
                }
                auto inputbind = _input_action_bindings.find(name);
                if (inputbind != _input_action_bindings.end())
                {
                    auto directbind = _input_map.find(inputbind->second);
                    if (directbind != _input_map.end())
                    {
                        _input_map.erase(directbind);
                    }
                    _input_action_bindings.erase(inputbind);
                }
            }

            /// Adds an action that is not bound to an identifier
            void AddBindlessAction(InputAction action)
            {
                /// No duplicates checks, but why would you add multiple identical actions?!
                _any_actions.push_back(action);
            }

            /// Removes a bindless action
            void RemoveBindlessAction(InputAction action)
            {
                for (auto i = _any_actions.begin(); i != _any_actions.end(); i++)
                {
                    if (*i == action)
                    {
                        _any_actions.erase(i);
                        return;
                    }
                }
            }

            /// Binds an action to a specified input condition
            void BindAction(string action, InputIdent condition)
            {
                auto itr = _action_bindings.find(action);
                if (itr == _action_bindings.end())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Input handler attempted to bind action '%s' but this action has not been added to the handler!", action.c_str());
                }
                else
                {
                    /// Clean up the input map
                    auto old_bind = _input_action_bindings.find(action);
                    if (old_bind != _input_action_bindings.end())
                    {
                        auto old_input = _input_map.find((*old_bind).second);
                        if (old_input != _input_map.end())
                        {
                            _input_map.erase(old_input);
                        }
                    }
                    _input_action_bindings[action] = condition;
                    _input_map[condition] = (*itr).second;
                }
            }

            const InputIdent* GetActionBind(string action)
            {
                auto itr = _action_bindings.find(action);
                if (itr != _action_bindings.end())
                {
                    return &((*itr).second);
                }
                SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Could not find binding value for action '%s'.", action.c_str());
                return nullptr;
            }

            /// Removes all actions, bindings and input data from this handler
            void Clear()
            {
                _input_action_bindings.clear();
                _action_bindings.clear();
                _input_map.clear();
                _input_state_bindings.clear();
            }

        protected:
            /// Updates the state of the associated input ident, if it finds it.
            void UpdateState(const InputIdent& ident, bool state)
            {
                /// Set relevant state
                auto itr = _state_map.find(ident);
                if (itr != _state_map.end())
                {
                    itr->second = state;
                }
            }

            /// Call the action associated with an input; returns true if successful
            ActionOutcome CallAction(const InputData& data, const InputIdent& ident)
            {
                /// Call all actions that accept any old data, regardless of identifier first
                for (auto i = _any_actions.begin(); i != _any_actions.end(); i++)
                {
                    ActionOutcome outcome = (*i)(data);
                    if (outcome != ActionOutcome::Ignore)
                    {
                        //SDL_Log("ActionOutcome returned outcome %s.", GetActionOutcomeName(outcome).c_str());
                        return outcome;
                    }
                }
                auto actionItr = _input_map.find(ident);
                if (actionItr != _input_map.end())
                {
                    /// Call the action and pass in the input data
                    ActionOutcome outcome = (*actionItr).second(data);
                    if (outcome != ActionOutcome::Ignore)
                    {
                        //SDL_Log("ActionOutcome returned outcome %s.", GetActionOutcomeName(outcome).c_str());
                        return outcome;
                    }
                }
                return ActionOutcome::Ignore;
            }

            /// Bindless actions - these actions or not bound to any particular identifier.
            /// When there are no corresponding actions for an input which is of the relevant type,
            /// these actions will be called. Useful for things like custom control bindings in a game,
            /// where you just need the data from any mouse or keyboard event to obtain an identifier such as a key code.
            vector<InputAction> _any_actions;

            /// Direct map of inputs to actions purely for fast lookup at runtime; changes when _input_action_bindings is changed.
            unordered_map<InputIdent, InputAction> _input_map;

            /// Direct map of inputs to states for fast lookup at runtime; changes when _input_state_bindings is changed.
            unordered_map<InputIdent, bool> _state_map;

        private:
            /// Input data bindings for states
            unordered_map<string, InputIdent> _input_state_bindings;

            /// Input data bindings for actions
            unordered_map<string, InputIdent> _input_action_bindings;

            /// Constant bindings of names to actions; this shouldn't be changed once all actions are added
            unordered_map<string, InputAction> _action_bindings;

        };

        /// A game might have multiple input contexts; for example, button X closes a dialog box when talking to NPCs,
        /// but the X button opens the player's inventory in normal gameplay.
        class InputContext
        {
        public:
            ~InputContext()
            {
                Clear();
            }

            /// Instantiates a new input handler and adds it to the list.
            template<class T>
            T* AddHandler()
            {
                auto itr = inputs.find(GetInputHandlerType<T>());
                if (itr == inputs.end())
                {
                    BaseInputHandler* handler = reinterpret_cast<BaseInputHandler*>(new T());
                    inputs[GetInputHandlerType<T>()] = handler;
                    return reinterpret_cast<T*>(handler);
                }
                else
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to add an input handler to a context, but the context already has an input handler of type [%d].", GetInputHandlerType<T>());
                }
                return GetHandler<T>();
            }

            /// Returns the specified handler
            template<class T>
            T* GetHandler()
            {
                auto itr = inputs.find(GetInputHandlerType<T>());
                if (itr == inputs.end())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to get input handler of type [%d] from a context, but it has not been added to the context!", GetInputHandlerType<T>());
                    return nullptr;
                }
                return reinterpret_cast<T*>((*itr).second);
            }

            /// Pass an input event to all the handlers
            ActionOutcome HandleInput(const SDL_Event& raw)
            {
                if (active)
                {
                    for (auto i = inputs.begin(); i != inputs.end(); i++)
                    {
                        /// Once handled, we can early-out and not bother passing the event to other handlers
                        ActionOutcome outcome = (*i).second->HandleInput(raw);
                        if (outcome != ActionOutcome::Ignore)
                        {
                            return outcome;
                        }
                    }
                }
                return ActionOutcome::Ignore;
            }

            /// Is this context currently active?
            bool IsActive()
            {
                return active;
            }

            /// Enable or disable this input context
            void SetActive(bool _active)
            {
                active = _active;
            }

            /// Destroys all input handlers associated with this input context
            void Clear()
            {
                for (auto i = inputs.begin(); i != inputs.end(); i++)
                {
                    if ((*i).second != nullptr)
                    {
                        delete (*i).second;
                        (*i).second = nullptr;
                    }
                }
                inputs.clear();
            }

        private:
            /// List of input handlers e.g. keyboard handler, mouse handler, joystick handler etc.
            unordered_map<InputHandlerType, BaseInputHandler*> inputs;

            /// When true, the input system will pass input data to this context
            bool active = true;

        };

        class InputController
        {
        public:
            InputController();
            ~InputController();

            /// Adds an input context; ideally do this just once when starting Ossium
            void AddContext(string name, InputContext* context);

            /// Removes a specified input context
            void RemoveContext(string name);

            /// This polls all input events and passes them to the currently active input contexts
            void Update();

            /// Passes a single event to all contexts. Useful if you don't exclusively use InputHandlers to handle SDL events
            /// Returns true if the event was handled
            ActionOutcome HandleEvent(const SDL_Event& raw);

            /// Removes all input contexts
            void Clear();

        private:
            /// All contexts by name
            unordered_map<string, InputContext*> contexts;

            NOCOPY(InputController);

        };

    }

}

#endif // INPUT_H
