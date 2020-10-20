#include "ViewCard.h"

namespace Ossium
{

    REGISTER_COMPONENT(ViewCard);

    void ViewCard::Show()
    {
        if (!entity->IsActiveLocally())
        {
            OnShow(*this);
            entity->SetActive(true);
            OnShown(*this);
        }
    }

    void ViewCard::Hide()
    {
        if (entity->IsActiveLocally())
        {
            OnHide(*this);
            entity->SetActive(false);
            OnHidden(*this);
        }
    }

}
