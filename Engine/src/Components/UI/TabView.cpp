#include "ViewCard.h"
#include "TabView.h"
#include "TabButton.h"

using namespace std;

namespace Ossium
{

    REGISTER_COMPONENT(TabView);

    void TabView::OnLoadFinish()
    {
    #ifndef OSSIUM_EDITOR
        // TODO: serialise in scene instead when possible.
        cards = entity->GetComponentsInChildren<ViewCard>();
        for (unsigned int i = 0; i < cards.size(); i++)
        {
            i == currentTab ? cards[i]->Show() : cards[i]->Hide();
        }
        tabButtons = entity->GetComponentsInChildren<TabButton>();
        for (unsigned int i = 0; i < tabButtons.size(); i++)
        {
            // Call OnLoadFinish() early to ensure the texture is loaded.
            tabButtons[i]->sprite->OnLoadFinish();
            tabButtons[i]->SetSelected(i == currentTab);
            tabButtons[i]->OnClicked += [=] (Button& button) { this->SwitchTo(i); };
        }
    #endif
    }

    void TabView::SwitchTo(unsigned int tab)
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
            else
            {
                Log.Warning("Tab index {0} is out of range.", tab);
            }
            
            if (currentTab < cards.size())
            {
                cards[currentTab]->Hide();
            }
            if (tab < cards.size())
            {
                cards[tab]->Show();
            }
            
            currentTab = tab;
            OnSwitchTab(*this);
        }
    }

    unsigned int TabView::GetCurrentTab()
    {
        return currentTab;
    }

    ViewCard* TabView::GetCurrentTabCard()
    {
        return currentTab < cards.size() ? cards[currentTab] : nullptr;
    }

}
