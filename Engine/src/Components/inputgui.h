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
    class OSSIUM_EDL InteractableGUI : public GraphicComponent
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
        DECLARE_ABSTRACT_COMPONENT(InteractableGUI);

        /// This does nothing, but should be implemented in the child class.
        virtual void Render(Renderer& renderer) = 0;

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
        /// Called whenever the pointer moves while this element is pressed.
        virtual void OnDrag(const MouseInput& data);
        /// Called when the mouse is scrolled, regardless of whether this element is currently hovered.
        virtual void OnScroll(const MouseInput& data);

        /// Method that detects whether the pointer is touching this GUI element.
        virtual bool ContainsPointer(Point position);

    private:
        void OnPointerEvent(const MouseInput& data);

        bool pressed = false;
        bool hovered = false;

    };

    /// Handles inputs for all attached or registered UI elements, such as buttons and sliders.
    class OSSIUM_EDL InputGUI : public BaseComponent, public InputContext
    {
    public:
        DECLARE_COMPONENT(BaseComponent, InputGUI);

        /// Sets up the input context and all bindings accordingly.
        void OnCreate();

        /// Adds an interactable GUI element to a context.
        void AddInteractable(StrID context, InteractableGUI& element);

        /// Removes a specific interactable GUI element. Returns true if the element exists in the specified context and is removed successfully.
        bool RemoveInteractable(StrID context, InteractableGUI& element);

        /// Removes an entire GUI context. Returns true if the context exists and is removed successfully.
        bool RemoveContext(StrID context);

        /// Returns array of references to all the GUI elements in a particular context.
        /// Returns null if the context does not exist.
        vector<InteractableGUI*>* GetContext(StrID context);

        /// Returns the current context.
        vector<InteractableGUI*>* GetCurrentContext();

        /// Removes all contexts from the lookup table.
        void RemoveAll();

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
        /// TODO: Add true directional navigation support, possibly via Lambdas or a 2D array
        ActionOutcome SelectUp(const KeyboardInput& data);
        ActionOutcome SelectDown(const KeyboardInput& data);
        ActionOutcome SwitchContextForward(const KeyboardInput& data);
        ActionOutcome SwitchContextBack(const KeyboardInput& data);
        ActionOutcome GoBack(const KeyboardInput& data);

        /// The current GUI context
        StrID currentContext = nullptr;
        unsigned int currentContextIndex = 0;

        /// Lookup table for GUI elements in different GUI contexts.
        /// Uses string interning for fast key hashing.
        map<StrID, pair<vector<InteractableGUI*>, unsigned int>> contextLookup;
        /// Used to maintain GUI context order, where contexts added later are further down the navigation chain.
        vector<StrID> contextOrder;

    };

}

#endif // INPUTGUI_H
