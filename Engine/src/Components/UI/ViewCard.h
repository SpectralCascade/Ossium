#ifndef VIEWCARD_H
#define VIEWCARD_H

#include "../../Core/component.h"

namespace Ossium
{

    class ViewCard : public Component
    {
    public:
        DECLARE_ABSTRACT_COMPONENT(Component, ViewCard);

        /// Hides the view
        void Show();

        /// Shows the view
        void Hide();
        
    };

}


#endif // VIEWCARD_H
