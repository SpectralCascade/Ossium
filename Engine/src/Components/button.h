#ifndef BUTTON_H
#define BUTTON_H

#include "inputgui.h"
#include "statesprite.h"

namespace Ossium
{

    class Button : public InteractableGUI
    {
    public:
        DECLARE_COMPONENT(InteractableGUI, Button);

        void OnLoadFinish();

        /// Override to use sprite rect.
        bool ContainsPointer(Point position);

        /// Callback registry that is called whenever the button is clicked.
        Callback<Button&> OnClicked;

        /// Pointer to implementation of IOnButtonState.
        IOnButtonState* stateFollower = nullptr;

        /// Pointer to implementation of IContainsPoint.
        IContainsPoint* hitTester = nullptr;

    protected:
        void OnClick();
        void OnHoverBegin();
        void OnHoverEnd();
        void OnPointerDown();
        void OnPointerUp();

    };

}

#endif // BUTTON_H
