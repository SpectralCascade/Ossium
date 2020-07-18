#include "editorcontroller.h"
#include "editorlayout.h"
#include "../Windows/ToolBar.h"
#include "contextmenu.h"

namespace Ossium::Editor
{

    EditorController::EditorController(ResourceController* resources)
    {
        this->resources = resources;
        input = new InputController();
        mainLayout = new EditorLayout(this, "Ossium Editor");
        toolbar = mainLayout->Add<ToolBar>(DockingMode::TOP);
        mainLayout->GetNativeWindow()->OnCloseButton += [&] (Window& caller) {
            running = false;
        };
    }

    EditorController::~EditorController()
    {
        for (auto layout : layouts)
        {
            delete layout;
        }
        layouts.clear();
        delete mainLayout;
        delete input;
    }

    EditorController::CustomMenuButton::CustomMenuButton(string path, function<void()> func)
    {
        menuPath = path;
        onClick = func;
    }

    void EditorController::AddCustomMenu(string menuPath, function<void()> onClick)
    {
        customMenuTools[menuPath] = onClick;
    }

    EditorLayout* EditorController::CreateLayout()
    {
        EditorLayout* layout = new EditorLayout(this, "Ossium Editor");
        layouts.push_back(layout);
        return layout;
    }

    void EditorController::RemoveLayout(EditorLayout* layout)
    {
        if (layout != nullptr)
        {
            for (auto itr = layouts.begin(); itr != layouts.end(); itr++)
            {
                if (*itr == layout)
                {
                    layouts.erase(itr);
                    delete layout;
                    break;
                }
            }
        }
        if (layout == mainLayout)
        {
            Log.Warning("Attempted to remove main layout from an EditorController instance! Quitting editor...");
            running = false;
        }
    }

    bool EditorController::Update()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
                break;
            }

            // Update native layout windows
            mainLayout->HandleEvent(e);
            for (auto layout : layouts)
            {
                layout->HandleEvent(e);
            }

            // Update the context menu
            ContextMenu::HandleInput(e);

            // Handle standard input events.
            input->HandleEvent(e);
        }

        // Update the GUI
        mainLayout->Update();
        for (auto layout : layouts)
        {
            layout->Update();
        }

        ContextMenu::GetMainInstance(resources)->Update();

        if (toolbar->ShouldQuit())
        {
            running = false;
        }

        return running;
    }

    ResourceController* EditorController::GetResources()
    {
        return resources;
    }

    InputController* EditorController::GetInput()
    {
        return input;
    }

}
