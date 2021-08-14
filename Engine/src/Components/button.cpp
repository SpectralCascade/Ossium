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
    }

    bool Button::ContainsPointer(Point position)
    {
        if (hitTester == nullptr)
        {
            Log.Error("Failed to find Button hit tester implementation! Disabling Button on entity \"{0}\"", entity->name);
            SetEnabled(false);
            return false;
        }
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
