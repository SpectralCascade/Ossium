#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <vector>
#include <SDL.h>

#include "helpermacros.h"

using namespace std;

namespace Ossium
{

    inline namespace input
    {

        typedef Uint32 InputHandlerType;

        namespace internals
        {

            /// This works in a similar way to the ECS
            class HandlerRegistry
            {
            private:
                static InputHandlerType nextTypeIdent;
                InputHandlerType typeIdent;

            public:
                HandlerRegistry()
                {
                    typeIdent = nextTypeIdent;
                    nextTypeIdent++;
                }

                const InputHandlerType getType()
                {
                    return typeIdent;
                }

                static Uint32 GetTotalTypes()
                {
                    return (Uint32)nextTypeIdent;
                }
            };

        }

        /// Declares an input handler type
        /// Add this to the end of any class you wish to register as a component
        #define DECLARE_INPUT_HANDLER(TYPE) public: static internals::HandlerRegistry __input_type_entry_

        /// Adds the input handler type to the registry by static instantiation
        /// Add this to the class definition of an input handler that uses DECLARE_INPUT_HANDLER
        #define REGISTER_INPUT_HANDLER(TYPE) internals::HandlerRegistry TYPE::__input_type_entry_

        /// Constant return type id for a specified input handler type
        template<class T>
        InputHandlerType getInputHandlerType()
        {
            return T::__input_type_entry_.getType();
        }

        /// Interface for accessing InputHandler
        class BaseInputHandler
        {
        public:
            virtual bool HandleInput(const SDL_Event& raw) = 0;
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
            typedef void (*InputAction)(const InputData& data);

            virtual ~InputHandler()
            {
                Clear();
            }

            /// Takes raw input data and strips away the irrelevant stuff, then checks if it matches anything in the actions table
            /// Returns true if the raw event data should be discarded after handling
            virtual bool HandleInput(const SDL_Event& raw) = 0;

            /// This method adds a callback for a specific game action
            void AddAction(string name, InputAction action)
            {
                _action_bindings[name] = action;
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
            void Bind(string action, InputIdent condition)
            {
                auto itr = _action_bindings.find(action);
                if (itr == _action_bindings.end())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Input handler attempted to bind action '%s' but this action has not been added to the handler!", action.c_str());
                }
                else
                {
                    /// Clean up the input map
                    auto old_bind = _input_bindings.find(action);
                    if (old_bind != _input_bindings.end())
                    {
                        auto old_input = _input_map.find((*old_bind).second);
                        if (old_input != _input_map.end())
                        {
                            _input_map.erase(old_input);
                        }
                    }
                    _input_bindings[action] = condition;
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
                _input_bindings.clear();
                _action_bindings.clear();
                _input_map.clear();
            }

        protected:
            /// Call the action associated with an input; returns true if successful
            bool CallAction(const InputData& data, const InputIdent& ident)
            {
                /// Call all actions that accept any old data, regardless of identifier first
                for (auto i = _any_actions.begin(); i != _any_actions.end(); i++)
                {
                    (*i)(data);
                }
                auto actionItr = _input_map.find(ident);
                if (actionItr != _input_map.end())
                {
                    /// Call the action and pass in the input data
                    (*actionItr).second(data);
                    return true;
                }
                return false;
            }

            /// Bindless actions - these actions or not bound to any particular identifier
            /// When there are no corresponding actions for an input which is of the relevant type,
            /// these actions will be called. Useful for things like custom control bindings in a game,
            /// where you just need the data from any mouse or keyboard event to obtain an identifier such as a key code
            vector<InputAction> _any_actions;

            /// Direct map of inputs to actions purely for fast lookup at runtime; changes when _input_bindings is changed
            unordered_map<InputIdent, InputAction> _input_map;

        private:
            /// Input data bindings
            unordered_map<string, InputIdent> _input_bindings;

            /// Constant bindings of names to actions; this shouldn't be changed once all actions are added
            unordered_map<string, InputAction> _action_bindings;

        };

        /// A game might have multiple input contexts; for example, button X closes a dialog box when talking to NPCs,
        /// but the X button opens the player's inventory in another context
        class InputContext
        {
        public:
            InputContext()
            {
                active = true;
            }
            ~InputContext()
            {
                Clear();
            }

            /// Instantiates a new input handler and adds it to the list.
            template<class T>
            T* AddHandler()
            {
                auto itr = inputs.find(getInputHandlerType<T>());
                if (itr == inputs.end())
                {
                    BaseInputHandler* handler = reinterpret_cast<BaseInputHandler*>(new T());
                    inputs[getInputHandlerType<T>()] = handler;
                    return reinterpret_cast<T*>(handler);
                }
                else
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to add an input handler to a context, but the context already has an input handler of type [%d].", getInputHandlerType<T>());
                }
                return GetHandler<T>();
            }

            /// Returns the specified handler
            template<class T>
            T* GetHandler()
            {
                auto itr = inputs.find(getInputHandlerType<T>());
                if (itr == inputs.end())
                {
                    SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "Attempted to get input handler of type [%d] from a context, but it has not been added to the context!", getInputHandlerType<T>());
                    return nullptr;
                }
                return reinterpret_cast<T*>((*itr).second);
            }

            /// Pass an input event to all the handlers
            bool HandleInput(const SDL_Event& raw)
            {
                if (!active)
                {
                    return false;
                }
                for (auto i = inputs.begin(); i != inputs.end(); i++)
                {
                    /// Once handled, we can early-out and not bother passing the event to other handlers
                    if ((*i).second->HandleInput(raw))
                    {
                        return true;
                    }
                }
                return false;
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
            bool active;

            NOCOPY(InputContext);

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
            bool HandleEvent(const SDL_Event& raw);

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
