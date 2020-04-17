#ifndef BUTTON_H
#define BUTTON_H

#include "../Ossium.h"

namespace Ossium
{

    class Button : public InteractableGUI
    {
    public:
        DECLARE_COMPONENT(InteractableGUI, Button);

        /// Adds a sprite component and sets up a reference to it.
        void OnCreate();

        /// Does nothing because we have a separate sprite component for the actual graphics.
        void Render(Renderer& renderer);

        /// Override to use sprite rect.
        bool ContainsPointer(Point position);

        /// Callback registry that is called whenever the button is clicked
        Callback<Button> OnClicked;

        /// Reference to the sprite component this button requires.
        StateSprite* sprite = nullptr;

    protected:
        void OnClick();
        void OnHoverBegin();
        void OnHoverEnd();
        void OnPointerDown();
        void OnPointerUp();

    };

}

#endif // BUTTON_H
