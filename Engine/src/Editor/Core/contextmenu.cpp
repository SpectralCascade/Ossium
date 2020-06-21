#include "contextmenu.h"

namespace Ossium::Editor
{

    ContextMenu::Option(string text, function<void(void)> onClick, ContextMenu* expansion, bool enabled)
    {
        this->text = text;
        this->onClick = onClick;
        this->expansion = expansion;
        this->enabled = enabled;
    }

    ContextMenu::ContextMenu(ResourceController* resources)
    {
        nativeWindow = new Window("Menu", 20, 10, false, SDL_WINDOW_HIDDEN | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_BORDERLESS);
        nativeRenderer = new Renderer(nativeWindow);
        nativeInput = new InputController();
        Init(new InputContext(), resources);
    }

    void ContextMenu::Add(string text, function<void(void)> onClick, bool enabled = true)
    {
        options.push_back(Option(text, onClick, nullptr, enabled));
    }

    void ContextMenu::Add(string text, ContextMenu* expansion, bool enabled = true)
    {
        options.push_back(Option(text, [] () {}, expansion, enabled);
    }

    void ContextMenu::SetOptions(vector<Option> options)
    {
        this->options = options;
    }

    void ContextMenu::Clear()
    {
        options.clear();
    }

}
