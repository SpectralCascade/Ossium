#include "../button.h"

namespace Ossium
{

    class TabView;

    struct TabButtonSchema : public Schema<TabButtonSchema, 20>
    {
        DECLARE_BASE_SCHEMA(TabButtonSchema, 20);

    };

    class TabButton : public Button, public TabButtonSchema
    {
    public:
        CONSTRUCT_SCHEMA(Button, TabButtonSchema);
        DECLARE_COMPONENT(Button, TabButton);

        bool IsSelected();
        void SetSelected(bool select);

    protected:
        void OnHoverBegin();
        void OnHoverEnd();
        void OnPointerDown();
        void OnPointerUp();
        void OnClick();

    private:
        bool selected = false;

    };

}
