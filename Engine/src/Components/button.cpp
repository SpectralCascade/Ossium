#include "button.h"
#include "statesprite.h"

namespace Ossium
{

    REGISTER_COMPONENT(Button);

    void Button::OnLoadFinish()
    {
        ParentType::OnLoadFinish();
        /// We can't inherit from StateSprite as we already inherit indirectly from GraphicComponent,
        /// but we can add our own instance to the ECS.
        sprite = entity->AddComponentOnce<StateSprite>();
    }

    bool Button::ContainsPointer(Point position)
    {
        return sprite->GetRect(GetTransform()->GetWorldPosition()).Contains(position);
    }

    void Button::OnClick()
    {
        if (IsEnabled())
        {
            sprite->ChangeSubState(1);
            OnClicked(*this);
        }
    }

    void Button::OnPointerDown()
    {
        if (IsEnabled())
        {
            sprite->ChangeSubState(2);
        }
    }

    void Button::OnPointerUp()
    {
        if (IsEnabled())
        {
            sprite->ChangeSubState(0);
        }
    }

    void Button::OnHoverBegin()
    {
        if (IsEnabled())
        {
            if (!IsPressed())
            {
                sprite->ChangeSubState(1);
            }
        }
    }

    void Button::OnHoverEnd()
    {
        if (!IsPressed())
        {
            sprite->ChangeSubState(0);
        }
    }

}
