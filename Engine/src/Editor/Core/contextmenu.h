#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <Ossium.h>

namespace Ossium::Editor
{

    /// Represents a context menu, which can be used for drop downs and other editing tools.
    class ContextMenu : public NeuronGUI
    {
    public:
        struct Option
        {
            Option(string text, function<void(void)> onClick, ContextMenu* expansion = nullptr, bool enabled = true);

            string text;
            ContextMenu* expansion;
            bool enabled;
            function<void(void)> onClick;
        };

        ContextMenu(ResourceController* resources);

        static ContextMenu* Get()

        void Add(string text, function<void(void)> onClick, bool enabled = true);
        void Add(string text, ContextMenu* expansion, bool enabled = true);

        void SetOptions(vector<Option> options);

        void Clear();

    protected:
        void OnGUI();

        NeuronClickableStyle style = NeuronStyles::NEURON_CONTEXT_OPTION_STYLE;

    private:
        vector<Option> options;

        Window* nativeWindow;
        Renderer* nativeRenderer;
        InputController* nativeInput;

        /// The main context menu instance, used to
        static ContextMenu* mainInstance = nullptr;

    };

}

#endif // CONTEXTMENU_H
