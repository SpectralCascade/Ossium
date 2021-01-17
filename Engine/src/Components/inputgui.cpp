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
#include "inputgui.h"

using namespace std;

namespace Ossium
{

    BaseComponent* InteractableGUI::ComponentFactory(void* target_entity)
    {
        return nullptr;
    }
    InteractableGUI::InteractableGUI() {}
    InteractableGUI::~InteractableGUI() {}
    void InteractableGUI::OnCreate() { ParentType::OnCreate(); }
    void InteractableGUI::OnSetActive(bool active) { ParentType::OnSetActive(active); }
    void InteractableGUI::OnSetEnabled(bool enable) { ParentType::OnSetEnabled(enable); }
    void InteractableGUI::OnLoadStart() { ParentType::OnLoadStart(); }
    void InteractableGUI::OnClone(BaseComponent* src) {}
    void InteractableGUI::Update(){}
    std::string InteractableGUI::GetBaseTypeNames()
    {
        return std::is_same<BaseComponent, ParentType>::value ?
            std::string("") : std::string(parentTypeName) + "," + ParentType::GetBaseTypeNames();
    }
    Ossium::TypeSystem::TypeFactory<BaseComponent, ComponentType> InteractableGUI::__ecs_factory_ = 
    std::is_same<ParentType, BaseComponent>::value ? Ossium::TypeSystem::TypeFactory<BaseComponent, ComponentType>(
        SID( "InteractableGUI" )::str, ComponentFactory
    ) :
    Ossium::TypeSystem::TypeFactory<BaseComponent, ComponentType>(
        SID( "InteractableGUI" )::str, ComponentFactory, std::string(parentTypeName), true
    );

    void InteractableGUI::OnLoadFinish()
    {
        ParentType::OnLoadFinish();
#ifndef OSSIUM_EDITOR
        input = entity->GetAncestor<InputGUI>();
        if (input == nullptr)
        {
            input = entity->GetComponent<InputGUI>();
        }
        if (input != nullptr)
        {
            input->AddInteractable(this);
        }
#endif
    }

    void InteractableGUI::OnDestroy()
    {
        if (input != nullptr)
        {
            input->RemoveInteractable(this);
        }
    }

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
    void InteractableGUI::OnPointerMove()
    {
    }
    void InteractableGUI::OnScroll()
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
        lastMousePos = mpos;
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
            OnScroll();
            break;
        case MOUSE_MOTION:
            OnPointerMove();
            break;
        default:
            break;
        }
    }

    Vector2 InteractableGUI::GetLastMousePosition()
    {
        return lastMousePos;
    }

    REGISTER_COMPONENT(InputGUI);

    void InputGUI::OnCreate()
    {
        Component::OnCreate();
        string context = Utilities::Format("InputGUI:{0}", this);
        if (GetService<InputController>()->GetContext(context) == nullptr)
        {
            GetService<InputController>()->AddContext(context, this);
        }

        /// TODO: Use a generic input handler that takes touch input as well as mouse input.
        /// Mouse actions setup
        pointer = AddHandler<MouseHandler>();
        mouseActionId = pointer->AddBindlessAction([&] (const MouseInput& data) { return HandlePointer(data); });
        /// Keyboard actions setup
        keyboard = AddHandler<KeyboardHandler>();
        keyboard->AddAction("confirm_selection", [&] (const KeyboardInput& data) { return this->ConfirmSelection(data); }, SDLK_RETURN);
        keyboard->AddAction("select_right", [&] (const KeyboardInput& data) { return this->SelectRight(data); }, SDLK_RIGHT);
        keyboard->AddAction("select_left", [&] (const KeyboardInput& data) { return this->SelectLeft(data); }, SDLK_LEFT);
        keyboard->AddAction("select_up", [&] (const KeyboardInput& data) { return this->SelectUp(data); }, SDLK_UP);
        keyboard->AddAction("select_down", [&] (const KeyboardInput& data) { return this->SelectDown(data); }, SDLK_DOWN);
        keyboard->AddAction("go_back", [&] (const KeyboardInput& data) { return this->GoBack(data); }, SDLK_ESCAPE);
    }
    
    void InputGUI::OnSetActive(bool active)
    {
        if (active && IsEnabled())
        {
            SetActive(active);
        }
        else
        {
            SetActive(false);
        }
    }

    void InputGUI::OnSetEnabled(bool enable)
    {
        if (IsActive() && enable)
        {
            SetActive(enable);
        }
        else
        {
            SetActive(false);
        }
    }

    void InputGUI::OnDestroy()
    {
        Component::OnDestroy();
        string context = Utilities::Format("InputGUI:{0}", this);
        if (GetService<InputController>()->GetContext(context) != nullptr)
        {
            GetService<InputController>()->RemoveContext(context);
        }
    }

    void InputGUI::AddInteractable(InteractableGUI* element)
    {
        // First check that it hasn't already been added
        for (auto interactable : interactables)
        {
            if (interactable == element)
            {
                Log.Warning("Attempted to add InteractableGUI element that had already been added! Entity Name: {0}", element->GetEntity()->name);
                return;
            }
        }
        interactables.push_back(element);
    }

    bool InputGUI::RemoveInteractable(InteractableGUI* element)
    {
        for (auto itr = interactables.begin(); itr != interactables.end(); itr++)
        {
            /// Check if it's the same instance
            if (*itr == element)
            {
                interactables.erase(itr);
                return true;
            }
        }
        return false;
    }

    void InputGUI::Clear()
    {
        currentIndex = 0;
        interactables.clear();
    }

    ActionOutcome InputGUI::HandlePointer(const MouseInput& data)
    {
        if (data.type != MOUSE_UNKNOWN)
        {
            for (auto interactable : interactables)
            {
                interactable->OnPointerEvent(data);
            }
        }
        /// Ignored as this is a bindless action.
        return Ignore;
    }

    ActionOutcome InputGUI::ConfirmSelection(const KeyboardInput& data)
    {
        if (data.state)
        {
            // This effectively fakes a click event for the currently selected GUI element
            if (!interactables.empty() && currentIndex < interactables.size())
            {
                if (interactables[currentIndex]->IsHovered())
                {
                    interactables[currentIndex]->OnPointerDown();
                    interactables[currentIndex]->OnPointerUp();
                    interactables[currentIndex]->OnClick();
                }
                else
                {
                    /// Highlight the selected GUI element before actually carrying out the OnClick() action,
                    /// to indicate which element is selected.
                    interactables[currentIndex]->hovered = true;
                    interactables[currentIndex]->OnHoverBegin();
                }
                return ClaimContext;
            }
        }
        return Ignore;
    }

    ActionOutcome InputGUI::SelectRight(const KeyboardInput& data)
    {
        if (data.state)
        {
            if (!interactables.empty())
            {
                /// TODO: wrapping support?
                if (interactables[currentIndex]->IsHovered())
                {
                    interactables[currentIndex]->OnHoverEnd();
                    currentIndex = Clamp(currentIndex + 1, 0, interactables.size() - 1);
                }
                interactables[currentIndex]->OnHoverBegin();
                interactables[currentIndex]->hovered = true;
                return ClaimContext;
            }
        }
        return Ignore;
    }

    ActionOutcome InputGUI::SelectLeft(const KeyboardInput& data)
    {
        if (data.state)
        {
            if (!interactables.empty())
            {
                if (interactables[currentIndex]->IsHovered())
                {
                    interactables[currentIndex]->OnHoverEnd();
                    currentIndex = Clamp(currentIndex - 1, 0, interactables.size() - 1);
                }
                interactables[currentIndex]->OnHoverBegin();
                interactables[currentIndex]->hovered = true;
                return ClaimContext;
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
