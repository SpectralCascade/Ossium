#include "contextmenu.h"

using namespace std;

namespace Ossium::Editor
{

    ContextMenu::Option::Option(string text, function<void(void)> onClick, bool enabled, Image* icon, ContextMenu* expansion)
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

        nativeWindow->OnFocusLost += [&] (Window& caller) {
            if (focus == nullptr || focus == this)
            {
                mainContextMenu->Hide();
            }
            else if (attached != focus || focus == this)
            {
                Hide();
            }
        };

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
        else
        {
            mainInput->RemoveContext(Utilities::Format("ContextMenu@{0}", this));
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
            mainContextMenu->HandleWindowEvent(e);
            mainInput->HandleEvent(e);
        }
    }

    void ContextMenu::HandleWindowEvent(SDL_Event& e)
    {
        if (!nativeWindow->HandleEvent(e) && attached != nullptr)
        {
            // Pass the event on to the next context menu
            attached->HandleWindowEvent(e);
        }
    }

    void ContextMenu::Refresh()
    {
        if (!IsShown())
        {
            return;
        }

        DEBUG_ASSERT(renderer != nullptr, "NULL renderer in context menu!");

        renderer->SetDrawColor(backgroundColor);
        SDL_RenderClear(renderer->GetRendererSDL());
        nativeOrigin = nativeWindow->GetPosition();
        EditorGUI::Refresh();
        //Log.Info("InputState = {0}", InputState.ToString());
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

        if (attached != nullptr && attached->IsShown())
        {
            attached->Refresh();
        }

        SDL_RenderPresent(renderer->GetRendererSDL());
    }

    void ContextMenu::OnGUI()
    {
        for (Option& option : options)
        {
            Rect displayRect = nativeWindow->GetDisplayBounds();

            if (!IsVisible())
            {
                if (nativeWindow->GetHeight() >= (float)displayRect.h)
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
                    viewport = Rect(0, 0, width, displayRect.h).SDL();
                    renderer->SetViewportRect(viewport);
                    TriggerUpdate();
                }
            }

            Vector2 oldPos = GetLayoutPosition();

            // Setup text layout for the button
            int xpadding = iconWidth + 4;
            int ypadding = 2;

            TextLayout tlayout;
            Vector2 limits = Vector2(viewport.w - oldPos.x - xpadding, renderer->GetHeight() - ypadding);
            tlayout.SetPointSize(EditorStyle::EDITOR_CONTEXT_OPTION_STYLE.normalStyleText.ptsize);
            tlayout.SetBounds(limits);
            tlayout.mainColor = option.enabled ? EditorStyle::EDITOR_CONTEXT_OPTION_STYLE.normalStyleText.fg : EditorStyle::EDITOR_CONTEXT_OPTION_DISABLED_STYLE.normalStyleText.fg;
            tlayout.mainStyle = option.enabled ? EditorStyle::EDITOR_CONTEXT_OPTION_STYLE.normalStyleText.style : EditorStyle::EDITOR_CONTEXT_OPTION_DISABLED_STYLE.normalStyleText.style;
            tlayout.SetAlignment(Typographic::TextAlignment::LEFT_ALIGNED);
            Font& font = *resources->Get<Font>(style.normalStyleText.fontPath, style.normalStyleText.ptsize);
            tlayout.SetText(*renderer, font, option.text, true);
            tlayout.Update(font);

            bool hovered = false;

            if (option.enabled)
            {
                if (Button(option.text, tlayout, EditorStyle::EDITOR_CONTEXT_OPTION_STYLE, false, xpadding, ypadding, true, &hovered))
                {
                    option.onClick();
                    if (option.expansion == nullptr)
                    {
                        mainContextMenu->Hide();
                        break;
                    }
                }
            }
            else
            {
                Button(option.text, tlayout, EditorStyle::EDITOR_CONTEXT_OPTION_DISABLED_STYLE, false, xpadding, ypadding, true);
            }

            if (hovered && attached != option.expansion)
            {
                HideAttached();
            }

            // Draw extra features
            if (option.icon != nullptr)
            {
                option.icon->Render(renderer->GetRendererSDL(), Rect(oldPos.x + 2, oldPos.y + 2, iconWidth, iconHeight).SDL());
            }
            if (option.expansion != nullptr && option.expansion != this)
            {
                // Check if hovered, if so show the pop-out menu.
                if (hovered && attached != option.expansion)
                {
                    attached = option.expansion;
                    attached->parent = this;
                    attached->Show(nativeWindow->GetPosition() +
                        Vector2(nativeWindow->GetWidth() * (nativeWindow->GetPosition().x - displayRect.x < displayRect.w - nativeWindow->GetWidth() ? 1.0f : -1.0f),
                                oldPos.y + viewport.y
                        )
                    );
                }

                // Draw an arrow indicating that the menu pops out.
                float arrowHeight = 9.0f;
                Vector2 arrowPos = oldPos + Vector2(renderer->GetWidth() - ((iconWidth - 4) / 2), ((GetLayoutPosition().y - oldPos.y) / 2) - (arrowHeight / 2.0f));
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
        EditorGUI::Init(input, resourceController);
    }

    void ContextMenu::Add(string text, function<void(void)> onClick, Image* icon, bool enabled)
    {
        options.push_back(Option(text, onClick, enabled, icon, nullptr));
    }

    ContextMenu* ContextMenu::AddPopoutMenu(string text, Image* icon, bool enabled)
    {
        ContextMenu* expansion = new ContextMenu();
        expansion->Init(mainInput, resources);
        options.push_back(Option(text, [] () {}, enabled, icon, expansion));
        return expansion;
    }

    void ContextMenu::SetOptionEnabled(unsigned int index, bool enable)
    {
        if (index < options.size())
        {
            options[index].enabled = enable;
        }
    }

    void ContextMenu::SetOptions(vector<Option> options)
    {
        fitRenderer = true;
        for (Option& option : options)
        {
            if (option.expansion != nullptr)
            {
                delete option.expansion;
            }
        }
        this->options = options;
    }

    vector<ContextMenu::Option> ContextMenu::GetOptions()
    {
        return options;
    }

    void ContextMenu::ClearOptions()
    {
        fitRenderer = true;
        for (Option& option : options)
        {
            if (option.expansion != nullptr)
            {
                delete option.expansion;
            }
        }
        options.clear();
    }

    void ContextMenu::Show(Vector2 position)
    {
        input->SetActive(true);
        nativeWindow->SetPosition(position);
        nativeWindow->Show();
        nativeWindow->Focus();
        focus = this;
        TriggerUpdate();
    }

    void ContextMenu::Hide()
    {
        if (focus == this)
        {
            focus = nullptr;
        }
        nativeWindow->Hide();
        HideAttached();
        if (parent != nullptr)
        {
            parent->attached = nullptr;
            parent = nullptr;
        }
        input->SetActive(false);
    }

    void ContextMenu::HideAttached()
    {
        if (attached != nullptr && attached->IsShown())
        {
            attached->Hide();
        }
    }

    bool ContextMenu::IsShown()
    {
        return nativeWindow->IsShown();
    }

    ContextMenu* ContextMenu::mainContextMenu = nullptr;
    ContextMenu* ContextMenu::focus = nullptr;
    int ContextMenu::iconWidth = 32;
    int ContextMenu::iconHeight = 32;
    int ContextMenu::width = 128;
    InputController* ContextMenu::mainInput = nullptr;
    ResourceController* ContextMenu::mainResources = nullptr;

}
