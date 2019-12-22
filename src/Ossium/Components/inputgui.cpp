/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
#include "inputgui.h"

using namespace std;

namespace Ossium
{

    inline namespace UI
    {

        REGISTER_ABSTRACT_COMPONENT(InteractableGUI, GraphicComponent);

        void InteractableGUI::OnHoverBegin()
        {
        }
        void InteractableGUI::OnHoverEnd()
        {
        }
        void InteractableGUI::OnPointerDown()
        {
        }
        void InteractableGUI::OnPointerUp()
        {
        }
        void InteractableGUI::OnDrag(const MouseInput& data)
        {
        }
        void InteractableGUI::OnScroll(const MouseInput& data)
        {
        }

        bool InteractableGUI::IsPressed()
        {
            return pressed;
        }

        bool InteractableGUI::IsHovered()
        {
            return hovered;
        }

        bool InteractableGUI::ContainsPointer(Point position)
        {
            return hitbox.Contains(position);
        }

        void InteractableGUI::OnPointerEvent(const MouseInput& data)
        {
            Point mpos;
            if (GetService<Renderer>() != nullptr)
            {
                /// Offsets to account for the aspect ratio / viewport of the window
                SDL_Rect vrect = GetService<Renderer>()->GetViewportRect();
                mpos = Point((float)(data.x - vrect.x), (float)(data.y - vrect.y));
            }
            else
            {
                mpos = Point((float)data.x, (float)data.y);
            }
            if (ContainsPointer(mpos))
            {
               if (!hovered)
               {
                   hovered = true;
                   OnHoverBegin();
               }
            }
            else if (hovered)
            {
                hovered = false;
                OnHoverEnd();
            }
            switch (data.type)
            {
            case MOUSE_BUTTON_LEFT:
                if (data.state)
                {
                    if (hovered && !pressed)
                    {
                        pressed = true;
                        OnPointerDown();
                    }
                }
                else
                {
                    if (pressed)
                    {
                        pressed = false;
                        OnPointerUp();
                        if (hovered)
                        {
                            OnClick();
                        }
                    }
                }
                break;
            case MOUSE_WHEEL:
                OnScroll(data);
                break;
            case MOUSE_MOTION:
                if (pressed)
                {
                    OnDrag(data);
                }
                break;
            default:
                break;
            }
        }

        REGISTER_COMPONENT(InputGUI);

        void InputGUI::OnCreate()
        {
            /// TODO: Use a generic input handler that takes touch input as well as mouse input.
            /// Mouse actions setup
            pointer = AddHandler<MouseHandler>();
            pointer->AddBindlessAction([&] (const MouseInput& data) { return HandlePointer(data); });
            /// Keyboard actions setup
            keyboard = AddHandler<KeyboardHandler>();
            keyboard->AddAction("confirm_selection", [&] (const KeyboardInput& data) { return this->ConfirmSelection(data); }, SDLK_RETURN);
            keyboard->AddAction("select_right", [&] (const KeyboardInput& data) { return this->SelectRight(data); }, SDLK_RIGHT);
            keyboard->AddAction("select_left", [&] (const KeyboardInput& data) { return this->SelectLeft(data); }, SDLK_LEFT);
            keyboard->AddAction("select_up", [&] (const KeyboardInput& data) { return this->SelectUp(data); }, SDLK_UP);
            keyboard->AddAction("select_down", [&] (const KeyboardInput& data) { return this->SelectDown(data); }, SDLK_DOWN);
            keyboard->AddAction("switch_context_forward", [&] (const KeyboardInput& data) { return this->SwitchContextForward(data); }, SDLK_TAB);
            keyboard->AddAction("switch_context_back", [&] (const KeyboardInput& data) { return this->SwitchContextBack(data); }, SDLK_BACKQUOTE);
            keyboard->AddAction("go_back", [&] (const KeyboardInput& data) { return this->GoBack(data); }, SDLK_ESCAPE);
        }

        void InputGUI::AddInteractable(StrID context, InteractableGUI& element)
        {
            auto itr = contextLookup.find(context);
            if (itr == contextLookup.end())
            {
                /// Add a new context
                contextOrder.push_back(context);
            }
            contextLookup[context].first.push_back(&element);
            if (currentContext == nullptr)
            {
                /// Set as the default context
                currentContext = context;
                contextLookup[context].second = 0;
            }
        }

        bool InputGUI::RemoveInteractable(StrID context, InteractableGUI& element)
        {
            auto itr = contextLookup.find(context);
            if (itr != contextLookup.end())
            {
                for (auto item : itr->second.first)
                {
                    /// Check if it's the same instance
                    if (item == &element)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        bool InputGUI::RemoveContext(StrID context)
        {
            auto itr = contextLookup.find(context);
            if (itr != contextLookup.end())
            {
                contextLookup.erase(itr);
                for (auto i = contextOrder.begin(); i != contextOrder.end(); i++)
                {
                    if (*i == context)
                    {
                        contextOrder.erase(i);
                        break;
                    }
                }
                if (currentContext == context)
                {
                    /// Move to a different context
                    KeyboardInput junkData;
                    junkData.state = true;
                    SwitchContextBack(junkData);
                }
                return true;
            }
            return false;
        }

        void InputGUI::RemoveAll()
        {
            contextLookup.clear();
            contextOrder.clear();
            currentContext = nullptr;
            currentContextIndex = 0;
        }

        vector<InteractableGUI*>* InputGUI::GetContext(StrID context)
        {
            auto i = contextLookup.find(context);
            if (i != contextLookup.end())
            {
                return &(i->second.first);
            }
            return nullptr;
        }

        vector<InteractableGUI*>* InputGUI::GetCurrentContext()
        {
            if (currentContext != nullptr)
            {
                return GetContext(currentContext);
            }
            return nullptr;
        }

        ActionOutcome InputGUI::HandlePointer(const MouseInput& data)
        {
            if (data.type != MOUSE_UNKNOWN)
            {
                for (auto i : contextLookup)
                {
                    for (auto element : i.second.first)
                    {
                        element->OnPointerEvent(data);
                    }
                }
            }
            /// Ignored as this is a bindless action.
            return Ignore;
        }

        ActionOutcome InputGUI::ConfirmSelection(const KeyboardInput& data)
        {
            if (data.state)
            {
                /// This effectively fakes a click event for the currently selected GUI element
                auto itr = contextLookup.find(currentContext);
                if (itr != contextLookup.end())
                {
                    unsigned int index = itr->second.second;
                    if (!itr->second.first.empty() && index < itr->second.first.size())
                    {
                        if (itr->second.first[index]->IsHovered())
                        {
                            itr->second.first[index]->OnClick();
                        }
                        else
                        {
                            /// Highlight the selected GUI element before actually carrying out the OnClick() action,
                            /// to indicate which element is selected.
                            itr->second.first[index]->hovered = true;
                            itr->second.first[index]->OnHoverBegin();
                        }
                        return ClaimContext;
                    }
                }
            }
            return Ignore;
        }

        ActionOutcome InputGUI::SelectRight(const KeyboardInput& data)
        {
            if (data.state)
            {
                auto itr = contextLookup.find(currentContext);
                if (itr != contextLookup.end())
                {
                    if (!itr->second.first.empty())
                    {
                        /// TODO: wrapping support?
                        if (itr->second.first[itr->second.second]->IsHovered())
                        {
                            itr->second.first[itr->second.second]->OnHoverEnd();
                            itr->second.second = Clamp(itr->second.second + 1, 0, itr->second.first.size() - 1);
                        }
                        itr->second.first[itr->second.second]->OnHoverBegin();
                        itr->second.first[itr->second.second]->hovered = true;
                        return ClaimContext;
                    }
                }
            }
            return Ignore;
        }

        ActionOutcome InputGUI::SelectLeft(const KeyboardInput& data)
        {
            if (data.state)
            {
                auto itr = contextLookup.find(currentContext);
                if (itr != contextLookup.end())
                {
                    if (!itr->second.first.empty())
                    {
                        if (itr->second.first[itr->second.second]->IsHovered())
                        {
                            itr->second.first[itr->second.second]->OnHoverEnd();
                            itr->second.second = Clamp(itr->second.second - 1, 0, itr->second.first.size() - 1);
                        }
                        itr->second.first[itr->second.second]->OnHoverBegin();
                        itr->second.first[itr->second.second]->hovered = true;
                        return ClaimContext;
                    }
                }
            }
            return Ignore;
        }

        ActionOutcome InputGUI::SelectUp(const KeyboardInput& data)
        {
            return SelectLeft(data);
        }

        ActionOutcome InputGUI::SelectDown(const KeyboardInput& data)
        {
            return SelectRight(data);
        }

        ActionOutcome InputGUI::SwitchContextForward(const KeyboardInput& data)
        {
            if (data.state)
            {
                if (contextOrder.empty())
                {
                    currentContextIndex = Wrap(currentContextIndex, 1, 0, contextOrder.size() - 1);
                    currentContext = contextOrder[currentContextIndex];
                    return ClaimContext;
                }
            }
            return Ignore;
        }

        ActionOutcome InputGUI::SwitchContextBack(const KeyboardInput& data)
        {
            if (data.state)
            {
                if (!contextOrder.empty())
                {
                    currentContextIndex = Wrap(currentContextIndex, -1, 0, !contextOrder.size() - 1);
                    currentContext = contextOrder[currentContextIndex];
                    return ClaimContext;
                }
            }
            return Ignore;
        }

        ActionOutcome InputGUI::GoBack(const KeyboardInput& data)
        {
            if (data.state)
            {
                OnBack(*this);
                return ClaimContext;
            }
            return Ignore;
        }

    }

}
