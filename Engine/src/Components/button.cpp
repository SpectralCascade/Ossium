#include "button.h"
#include "statesprite.h"

namespace Ossium
{

    REGISTER_COMPONENT(Button);

    void Button::OnCreate()
    {
        GraphicComponent::OnCreate();
        /// We can't inherit from StateSprite as we already inherit indirectly from GraphicComponent,
        /// but we can add our own instance to the ECS.
        sprite = entity->AddComponentOnce<StateSprite>();
    }

    void Button::Render(Renderer& renderer)
    {
    }

    bool Button::ContainsPointer(Point position)
    {
        return sprite->GetRect(GetTransform()->GetWorldPosition()).Contains(position);
    }

    void Button::OnClick()
    {
        sprite->ChangeSubState(1);
        OnClicked(*this);
    }

    void Button::OnPointerDown()
    {
        sprite->ChangeSubState(2);
    }

    void Button::OnPointerUp()
    {
        sprite->ChangeSubState(0);
    }

    void Button::OnHoverBegin()
    {
        if (!IsPressed())
        {
            sprite->ChangeSubState(1);
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
