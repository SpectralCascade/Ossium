#ifndef TABVIEW_H
#define TABVIEW_H

#include "../../Core/component.h"

using namespace std;

namespace Ossium
{

    // Forward declaration
    class TabButton;
    class ViewCard;

    class TabView : public Component
    {
    public:
        DECLARE_COMPONENT(Component, TabView);

        // Hookup tab buttons
        void OnLoadFinish();

        /// Closes the current card and shows the specified one.
        void SwitchTo(unsigned int tab);

        /// Returns the current tab index. Defaults to 0.
        unsigned int GetCurrentTab();

        /// Returns a pointer to the current view card. Return nullptr if there is no current tab.
        ViewCard* GetCurrentTabCard();

    private:
        // TODO: Serialise me when editor supports component serialisation
        vector<ViewCard*> cards;

        // Current tab
        unsigned int currentTab = 0;

        // Associated tab buttons
        vector<TabButton*> tabButtons;

    };

}


#endif // TABVIEW_H
