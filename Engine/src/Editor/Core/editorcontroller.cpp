#include "editorcontroller.h"
#include "editorlayout.h"
#include "../Windows/ToolBar.h"
#include "contextmenu.h"
#include "project.h"

using namespace std;

namespace Ossium::Editor
{

    EditorController::EditorController(ResourceController* resources)
    {
        this->resources = resources;
        input = new InputController();
        sceneInput = new InputController();
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
        delete sceneInput;
        delete input;
    }

    EditorController::MenuTool::MenuTool(string path, function<void()> onClick, function<bool()> isEnabled)
    {
        this->path = path;
        this->onClick = onClick;
        this->isEnabled = isEnabled;
    }

    void EditorController::AddCustomMenu(string menuPath, function<void()> onClick, function<bool()> isEnabled)
    {
        customMenuTools.push_back(MenuTool(menuPath, onClick, isEnabled));
    }

    Entity* EditorController::GetSelectedEntity()
    {
        return selectedEntity;
    }

    Scene* EditorController::GetSelectedScene()
    {
        return selectedScene;
    }

    vector<Scene*> EditorController::GetLoadedScenes()
    {
        vector<Scene*> result;
        Project* project = GetProject();
        if (project != nullptr)
        {
            for (auto scene : project->openScenes)
            {
                if (scene.loaded)
                {
                    Scene* loaded = resources->Get<Scene>(scene.path, GetMainLayout()->GetServices());
                    if (loaded != nullptr)
                    {
                        result.push_back(loaded);
                    }
                }
            }
        }
        return result;
    }

    void EditorController::SelectEntity(Entity* entity)
    {
        selectedEntity = entity;
    }

    void EditorController::SelectScene(Scene* scene)
    {
        selectedScene = scene;
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

    InputController* EditorController::GetSceneInput()
    {
        return sceneInput;
    }

    bool EditorController::Update()
    {
        Timer timer;
        timer.Start();

        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
                break;
            }

            // Update the context menu
            ContextMenu::HandleInput(e);

            // Update native layout windows
            mainLayout->HandleEvent(e);
            for (auto layout : layouts)
            {
                layout->HandleEvent(e);
            }

            // Handle standard input events.
            input->HandleEvent(e);

            // Finally, handle scene inputs.
            GetSceneInput()->HandleEvent(e);

        }

        // Update the GUI
        mainLayout->Update();
        for (auto layout : layouts)
        {
            layout->Update();
        }

        ContextMenu::GetMainInstance(resources)->Update();

        // Now delay about 16 ms to get ~60 FPS
        if (timer.GetTicks() < 16)
        {
            SDL_Delay(16 - timer.GetTicks());
        }

        if (toolbar->ShouldQuit())
        {
            running = false;
        }

        return running;
    }

    void EditorController::Quit()
    {
        running = false;
    }

    Font* EditorController::GetFont()
    {
        return resources->Get<Font>("assets/Orkney Regular.ttf", 12);
    }

    Project* EditorController::GetProject()
    {
        return loadedProject;
    }

    Project* EditorController::CreateProject()
    {
        CloseProject();
        loadedProject = new Project();
        return loadedProject;
    }

    Project* EditorController::OpenProject(string path)
    {
        CloseProject();
        loadedProject = new Project();
        loadedProject->Load(path);
        for (ListedScene& scene : loadedProject->openScenes)
        {
            if (scene.loaded)
            {
                // TODO: consider how the Scene is loaded/initialised; it is dependant on a renderer, which is limited to an individual window.
                // This isn't ideal, potentially scene loading can be done independently to avoid limiting the scene view to the main layout window?
                resources->LoadAndInit<Scene>(scene.path, mainLayout->GetServices());
            }
            else
            {
                resources->Free<Scene>(scene.path);
            }
        }
        loadedProject->SetPath(path);
        return loadedProject;
    }

    void EditorController::CloseProject()
    {
        if (loadedProject != nullptr)
        {
            delete loadedProject;
            loadedProject = nullptr;
        }
    }

    ResourceController* EditorController::GetResources()
    {
        return resources;
    }

    InputController* EditorController::GetInput()
    {
        return input;
    }

    EditorLayout* EditorController::GetMainLayout()
    {
        return mainLayout;
    }

}
