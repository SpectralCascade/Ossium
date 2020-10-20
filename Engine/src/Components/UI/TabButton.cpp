#include "TabButton.h"

namespace Ossium
{

    REGISTER_COMPONENT(TabButton);

    void TabButton::OnHoverBegin()
    {
        if (!selected && !IsPressed())
        {
            sprite->ChangeSubState(1);
        }
    }

    void TabButton::OnHoverEnd()
    {
        if (!selected && !IsPressed())
        {
            sprite->ChangeSubState(0);
        }
    }

    void TabButton::OnPointerDown()
    {
        sprite->ChangeSubState(2);
        // Behave as though the button is clicked for immediate effect.
        OnClicked(*this);
    }

    void TabButton::OnPointerUp()
    {
        if (!selected)
        {
            sprite->ChangeSubState(0);
        }
    }

    void TabButton::OnClick()
    {
        // Don't do anything here; tabs are activated on pointer down.
    }

    bool TabButton::IsSelected()
    {
        return selected;
    }

    void TabButton::SetSelected(bool select)
    {
        selected = select;
        sprite->ChangeSubState(selected ? 2 : 0);
    }

}
