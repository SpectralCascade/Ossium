#include "ViewCard.h"
#include "TabView.h"
#include "TabButton.h"

namespace Ossium
{

    REGISTER_COMPONENT(TabView);

    void TabView::OnLoadFinish()
    {
    #ifndef OSSIUM_EDITOR
        // TODO: serialise in scene instead when possible.
        tabButtons = entity->GetComponentsInChildren<TabButton>();
    #endif
    }

    void TabView::SwitchTo(unsigned int tab)
    {
        if (tab < cards.size())
        {
            if (currentTab != tab)
            {
                if (tab < tabButtons.size())
                {
                    tabButtons[tab]->SetSelected(true);
                    if (currentTab < tabButtons.size())
                    {
                        tabButtons[currentTab]->SetSelected(false);
                    }
                }
                if (currentTab < cards.size())
                {
                    cards[currentTab]->Hide();
                }
                cards[tab]->Show();
                currentTab = tab;
            }
        }
        else
        {
            Log.Warning("Tab index out of range, failed to switch to tab {0}.", tab);
        }
    }

    unsigned int TabView::GetCurrentTab()
    {
        return currentTab;
    }

    ViewCard* TabView::GetCurrentTabCard()
    {
        return (currentTab < cards.size() ? cards[currentTab] : nullptr);
    }

}
