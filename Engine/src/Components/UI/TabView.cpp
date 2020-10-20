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
        return currentTab < cards.size() ? cards[currentTab] : nullptr;
    }

}
