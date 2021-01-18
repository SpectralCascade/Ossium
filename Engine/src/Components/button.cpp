#include "button.h"
#include "UI/BoxLayout.h"
#include "statesprite.h"

namespace Ossium
{

    REGISTER_COMPONENT(Button);

    void Button::OnLoadFinish()
    {
        ParentType::OnLoadFinish();
        StateSprite* sprite = entity->GetComponent<StateSprite>();
        if (sprite != nullptr)
        {
            stateFollower = sprite;
            hitTester = sprite;
        }
        else
        {
            hitTester = entity->GetComponent<BoxLayout>();
        }

#ifndef OSSIUM_EDITOR
        DEBUG_ASSERT(hitTester != nullptr, "Button hit tester implementation was not found!");

        if (hitTester == nullptr)
        {
            Log.Error("Failed to find Button hit tester implementation! Disabling Button on entity \"{0}\"", entity->name);
            SetEnabled(false);
        }
#endif // OSSIUM_EDITOR
    }

    bool Button::ContainsPointer(Point position)
    {
        return hitTester->Contains(position);
    }

    void Button::OnClick()
    {
        if (stateFollower != nullptr)
        {
            stateFollower->OnButtonState(BUTTON_STATE_HOVERED);
        }
        OnClicked(*this);
    }

    void Button::OnPointerDown()
    {
        if (stateFollower != nullptr)
        {
            stateFollower->OnButtonState(BUTTON_STATE_PRESSED);
        }
    }

    void Button::OnPointerUp()
    {
        if (stateFollower != nullptr)
        {
            stateFollower->OnButtonState(BUTTON_STATE_NONE);
        }
    }

    void Button::OnHoverBegin()
    {
        if (!IsPressed() && stateFollower != nullptr)
        {
            stateFollower->OnButtonState(BUTTON_STATE_HOVERED);
        }
    }

    void Button::OnHoverEnd()
    {
        if (!IsPressed() && stateFollower != nullptr)
        {
            stateFollower->OnButtonState(BUTTON_STATE_NONE);
        }
    }

}
