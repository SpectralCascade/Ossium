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
#ifndef INPUTGUI_H
#define INPUTGUI_H

#include "../Core/input.h"
#include "../Core/keyboard.h"
#include "../Core/mouse.h"
#include "../Core/stringintern.h"
#include "../Core/component.h"
#include "../Core/callback.h"

namespace Ossium
{

    /// Forward declaration
    class InputGUI;

    /// GUI elements that can be clicked on can implement this interface
    class OSSIUM_EDL InteractableGUI : public Component
    {
    public:
        friend class InputGUI;

        /// A basic hit box for this GUI element. You can override the IsHovered() method
        Rect hitbox;

        /// Whether a pointer is currently clicking on this GUI element.
        bool IsPressed();
        /// Whether a pointer is currently hovering over this GUI element.
        bool IsHovered();

    protected:
        DECLARE_ABSTRACT_COMPONENT(Component, InteractableGUI);

        /// All GUI elements must implement OnClick() which is called after OnPointerUp() if this element is hovered over.
        virtual void OnClick() = 0;
        /// Called once when the pointer is touching this GUI element.
        virtual void OnHoverBegin();
        /// Called once after the pointer has stopped touching this GUI element.
        virtual void OnHoverEnd();
        /// Called once when this GUI element is pressed.
        virtual void OnPointerDown();
        /// Called once when this GUI element stops being pressed, even if not hovered.
        virtual void OnPointerUp();
        /// Called whenever the pointer moves.
        virtual void OnPointerMove();
        /// Called when the mouse is scrolled, regardless of whether this element is currently hovered.
        virtual void OnScroll();

        /// Method that detects whether the pointer is touching this GUI element.
        virtual bool ContainsPointer(Point position);

        // Virtual :D
        virtual void OnPointerEvent(const MouseInput& data);

        // Returns the last handled MouseInput event.
        Vector2 GetLastMousePosition();

        InputGUI* input = nullptr;

    private:
        // The last handled mouse input event.
        Vector2 lastMousePos;

        bool pressed = false;
        bool hovered = false;

    };

    /// Handles inputs for all attached or registered UI elements, such as buttons and sliders.
    class OSSIUM_EDL InputGUI : public Component, public InputContext
    {
    public:
        DECLARE_COMPONENT(Component, InputGUI);

        /// Sets up the input context and all bindings accordingly.
        void OnCreate();

        /// Override enables/disables the input context.
        void OnSetActive(bool active);
        
        /// Override enables/disables the input context.
        void OnSetEnabled(bool enable);

        /// Cleans up actions
        void OnDestroy();

        /// Adds an interactable GUI element to a context.
        void AddInteractable(InteractableGUI* element);

        /// Removes a specific interactable GUI element. Returns true if the element exists in the specified context and is removed successfully.
        bool RemoveInteractable(InteractableGUI* element);

        /// Removes all interactables.
        void Clear();

        /// Called when the GoBack() action is called (useful for things like the Android back button).
        Callback<InputGUI> OnBack;

    private:
        /// These are here for convenience; if you wish to use them directly you can use the inherited GetHandler<>() method.
        MouseHandler* pointer;
        KeyboardHandler* keyboard;

        /// Mouse input data distributor (non-bound).
        ActionOutcome HandlePointer(const MouseInput& data);
        /// Keyboard navigation methods
        ActionOutcome ConfirmSelection(const KeyboardInput& data);
        ActionOutcome SelectRight(const KeyboardInput& data);
        ActionOutcome SelectLeft(const KeyboardInput& data);
        /// TODO: Add true directional navigation support based on the entity position in the hierarchy
        ActionOutcome SelectUp(const KeyboardInput& data);
        ActionOutcome SelectDown(const KeyboardInput& data);
        ActionOutcome GoBack(const KeyboardInput& data);

        /// The current GUI context
        unsigned int currentIndex = 0;

        /// Handle for the bindless mouse action
        int mouseActionId;

        /// All interactables.
        std::vector<InteractableGUI*> interactables;

    };

}

#endif // INPUTGUI_H
