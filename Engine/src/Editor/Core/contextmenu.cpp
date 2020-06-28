#include "contextmenu.h"

namespace Ossium::Editor
{

    ContextMenu::Option::Option(string text, function<void(void)> onClick, Image* icon, ContextMenu* expansion, bool enabled)
    {
        this->text = text;
        this->onClick = onClick;
        this->icon = icon;
        this->expansion = expansion;
        this->enabled = enabled;
    }

    ContextMenu::ContextMenu()
    {
        backgroundColor = Color(240, 240, 240, 255);
        nativeWindow = new Window("Menu", width + (borderThickness * 2), 16, false, SDL_WINDOW_HIDDEN | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_BORDERLESS);
        renderer = new Renderer(nativeWindow);
    }

    ContextMenu::~ContextMenu()
    {
        for (Option& option : options)
        {
            if (option.expansion != nullptr)
            {
                delete option.expansion;
                option.expansion = nullptr;
            }
        }

        delete renderer;
        delete nativeWindow;
        if (mainContextMenu == this)
        {
            delete mainInput;
            delete mainResources;
        }
        delete input;
    }

    ContextMenu* ContextMenu::GetMainInstance(ResourceController* resourceController)
    {
        if (mainContextMenu == nullptr)
        {
            mainContextMenu = new ContextMenu();
            mainInput = new InputController();
            mainResources = resourceController;
            mainContextMenu->Init(mainInput, mainResources);
            mainContextMenu->nativeWindow->OnFocusLost += [&] (Window& caller) { mainContextMenu->Hide(); };
        }
        return mainContextMenu;
    }

    void ContextMenu::SetIconDimensions(int w, int h)
    {
        iconWidth = w;
        iconHeight = h;
    }

    int ContextMenu::GetIconWidth()
    {
        return iconWidth;
    }

    int ContextMenu::GetIconHeight()
    {
        return iconHeight;
    }

    void ContextMenu::SetMenuWidth(int width)
    {
        ContextMenu::width = width;
    }

    int ContextMenu::GetMenuWidth()
    {
        return width;
    }

    void ContextMenu::HandleInput(SDL_Event& e)
    {
        if (mainContextMenu != nullptr)
        {
            mainContextMenu->nativeWindow->HandleEvent(e);
            mainInput->HandleEvent(e);
        }
    }

    void ContextMenu::Refresh()
    {
        DEBUG_ASSERT(renderer != nullptr, "NULL renderer in context menu!");

        renderer->SetDrawColor(backgroundColor);
        SDL_RenderClear(renderer->GetRendererSDL());
        nativeOrigin = nativeWindow->GetPosition();
        NeuronGUI::Refresh();
        //Logger::EngineLog().Info("InputState = {0}", InputState.ToString());
        if (fitRenderer)
        {
            fitRenderer = false;
            nativeWindow->SetWidth(width);
            nativeWindow->SetHeight(GetLayoutPosition().y + (borderThickness * 2));
            viewport = Rect(borderThickness, borderThickness, width, GetLayoutPosition().y).SDL();
        }
        if (borderThickness > 0)
        {
            Rect border = Rect(0, 0, nativeWindow->GetWidth(), nativeWindow->GetHeight());
            renderer->SetViewportRect(border.SDL());
            border.Draw(*renderer, Colors::GRAY);
        }
        SDL_RenderPresent(renderer->GetRendererSDL());
    }

    void ContextMenu::OnGUI()
    {
        for (Option& option : options)
        {
            if (!IsVisible())
            {
                if (nativeWindow->GetHeight() >= (float)nativeWindow->GetDisplayHeight())
                {
                    // TODO: handle this case better - ideally prevent it.
                    break;
                }
                if (!fitRenderer)
                {
                    // In this scenario, we haven't got enough space to fit in this option! We must resize the renderer to maximum temporarily
                    // such that the options can be drawn and we can figure out exactly how much space is needed, resize the window + renderer to match
                    // and then trigger an update in the next frame.
                    fitRenderer = true;
                    viewport = Rect(0, 0, width, nativeWindow->GetDisplayHeight()).SDL();
                    renderer->SetViewportRect(viewport);
                    TriggerUpdate();
                }
            }

            Vector2 oldPos = GetLayoutPosition();

            if (Button(option.text, NeuronStyles::NEURON_CONTEXT_OPTION_STYLE, false, iconWidth + 4, 2, true))
            {
                option.onClick();
                Hide();
                break;
            }

            // Draw extra features
            if (option.icon != nullptr)
            {
                option.icon->Render(renderer->GetRendererSDL(), Rect(oldPos.x + 2, oldPos.y + 2, iconWidth, iconHeight).SDL());
            }
            if (option.expansion != nullptr && option.expansion != this)
            {
                bool hovered = false;
                // Check if hovered (we only care about the Y axis), if so show the pop-out menu.
                if (oldPos.y + InputState.mousePos.y < GetLayoutPosition().y && attached != option.expansion)
                {
                    hovered = true;

                    if (attached != nullptr)
                    {
                        attached->Hide();
                    }
                    attached = option.expansion;
                    attached->Show(nativeWindow->GetPosition() +
                        Vector2(nativeWindow->GetWidth() * (nativeWindow->GetPosition().x > nativeWindow->GetDisplayWidth() - nativeWindow->GetWidth() ? 1.0f : -1.0f),
                                oldPos.y - viewport.y
                        )
                    );

                }

                // Draw an arrow indicating that the menu pops out.
                float arrowHeight = 9.0f;
                Vector2 arrowPos = Vector2(viewport.x, viewport.y) + oldPos + Vector2(renderer->GetWidth() - ((iconWidth - 4) / 2), ((GetLayoutPosition().y - oldPos.y) / 2) - (arrowHeight / 2.0f));
                Line arrowLine = Line(arrowPos, arrowPos + Vector2(0, arrowHeight));

                renderer->SetDrawColor(hovered ? Colors::WHITE : Colors::BLACK);

                for (unsigned int i = 0, counti = (int)round(arrowHeight / 2.0f); i < counti; i++)
                {
                    arrowLine.Draw(*renderer);
                    arrowLine.a += Vector2::OneOne;
                    arrowLine.b += Vector2::OneNegOne;
                }

            }
        }

    }

    void ContextMenu::Init(InputController* inputController, ResourceController* resourceController)
    {
        nativeInput = inputController;
        input = new InputContext();
        nativeInput->AddContext(Utilities::Format("ContextMenu@{0}", this), input);
        NeuronGUI::Init(input, resourceController);
    }

    void ContextMenu::Add(string text, function<void(void)> onClick, Image* icon, bool enabled)
    {
        options.push_back(Option(text, onClick, icon, nullptr, enabled));
    }

    /*ContextMenu* ContextMenu::AddPopoutMenu(string text, Image* icon, bool enabled)
    {
        ContextMenu* expansion = new ContextMenu();
        expansion->Init(nativeInput, resources);
        options.push_back(Option(text, [] () {}, expansion, enabled));
        return expansion;
    }*/

    void ContextMenu::SetOptions(vector<Option> options)
    {
        this->options = options;
    }

    vector<ContextMenu::Option> ContextMenu::GetOptions()
    {
        return options;
    }

    void ContextMenu::Clear()
    {
        options.clear();
    }

    void ContextMenu::Show(Vector2 position)
    {
        nativeWindow->SetPosition(position);
        nativeWindow->Show();
        nativeWindow->Focus();
        TriggerUpdate();
    }

    void ContextMenu::Hide()
    {
        nativeWindow->Hide();
        if (attached != nullptr)
        {
            attached->Hide();
            attached = nullptr;
        }
    }

    ContextMenu* ContextMenu::mainContextMenu = nullptr;
    int ContextMenu::iconWidth = 32;
    int ContextMenu::iconHeight = 32;
    int ContextMenu::width = 128;
    InputController* ContextMenu::mainInput = nullptr;
    ResourceController* ContextMenu::mainResources = nullptr;

}
