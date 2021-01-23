#ifndef VIEWCARD_H
#define VIEWCARD_H

#include "../../Core/component.h"
#include "../../Core/callback.h"

namespace Ossium
{

    class ViewCard : public Component
    {
    public:
        DECLARE_COMPONENT(Component, ViewCard);

        /// Shows the view
        virtual void Show();

        /// Hides the view
        virtual void Hide();

        /// Called when Show() is called.
        Callback<ViewCard&> OnShow;

        /// Called when this view is fully shown.
        Callback<ViewCard&> OnShown;

        /// Called when Hide() is called.
        Callback<ViewCard&> OnHide;

        /// Called when this view is fully hidden.
        Callback<ViewCard&> OnHidden;
        
    };

}


#endif // VIEWCARD_H
