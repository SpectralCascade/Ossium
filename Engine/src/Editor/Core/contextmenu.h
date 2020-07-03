#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include "neurongui.h"

namespace Ossium::Editor
{

    /// Represents a context menu, which can be used for drop downs and other editing tools.
    class ContextMenu : public NeuronGUI
    {
    public:
        struct Option
        {
            Option(string text, function<void(void)> onClick, Image* icon = nullptr, ContextMenu* expansion = nullptr, bool enabled = true);

            string text;
            ContextMenu* expansion;
            bool enabled;
            function<void(void)> onClick;
            Image* icon = nullptr;
        };

        void Init(InputController* inputController, ResourceController* resourceController);

        /// Returns the main context menu.
        static ContextMenu* GetMainInstance(ResourceController* resourceController);

        /// Sets the dimensions of the option icons.
        static void SetIconDimensions(int w, int h);

        /// Returns the width of the option icons.
        static int GetIconWidth();
        /// Returns the height of the option icons.
        static int GetIconHeight();

        /// Sets the width of all context menus.
        static void SetMenuWidth(int width);
        /// Returns the width of all context menus.
        static int GetMenuWidth();

        /// Updates the main context menu.
        static void HandleInput(SDL_Event& e);

        /// Override that does some extra bits, such as drawing a border, setting the native window origin and refreshing any attached context menu.
        void Refresh();

        /// Adds an option to the context menu.
        void Add(string text, function<void(void)> onClick, Image* icon = nullptr, bool enabled = true);

        /// Adds an option to the context menu that pops out into another context menu when hovered (useful for grouping options).
        ContextMenu* AddPopoutMenu(string text, Image* icon = nullptr, bool enabled = true);

        /// Simply sets the options as a list directly.
        void SetOptions(vector<Option> options);

        /// Returns the set of options in this context menu.
        vector<Option> GetOptions();

        /// Clears all options in this context menu.
        void ClearOptions();

        /// Shows this context menu at an absolute position on screen.
        void Show(Vector2 position);

        /// Hides this context menu. Any open expansion menus connected to the options will also be hidden.
        void Hide();

        /// Returns true if this context menu is shown on screen.
        bool IsShown();

    protected:
        void OnGUI();

        NeuronClickableStyle style = NeuronStyles::NEURON_CONTEXT_OPTION_STYLE;

    private:
        /// Hides the attached context menu
        void HideAttached();

        /// Handles a given window event.
        void HandleWindowEvent(SDL_Event& e);

        vector<Option> options;

        // The context menu attached to this menu, if any.
        ContextMenu* attached = nullptr;
        // The parent context menu, to which this menu is attached.
        ContextMenu* parent = nullptr;

        Window* nativeWindow;
        InputController* nativeInput;

        // Should the window and renderer be resized to fit the options after the current refresh?
        bool fitRenderer = false;

        // The root context menu
        static ContextMenu* mainContextMenu;

        // The context menu that is currently in focus.
        static ContextMenu* focus;

        static int iconWidth;
        static int iconHeight;

        static int width;
        static const int borderThickness = 1;

        static InputController* mainInput;
        static ResourceController* mainResources;

        ContextMenu();
        virtual ~ContextMenu();

        NOCOPY(ContextMenu);

    };

}

#endif // CONTEXTMENU_H
