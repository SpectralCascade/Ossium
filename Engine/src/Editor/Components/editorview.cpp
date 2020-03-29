/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#include "editorview.h"
#include "../../Core/mousecursor.h"

namespace Ossium::Editor
{

    EditorWindow::~EditorWindow()
    {
        OnDestroy(*this);
        if (renderer != nullptr)
        {
            delete renderer;
            renderer = nullptr;
        }
        if (native != nullptr)
        {
            input->RemoveHandler<Window>();
            native = nullptr;
        }
    }

    void EditorWindow::Recreate()
    {
        if (renderer != nullptr)
        {
            delete renderer;
            renderer = nullptr;
        }

        if (parent != nullptr)
        {
            // If docked, no need to use the input context window, use the parent window instead.
            renderer = new Renderer(parent->GetNativeWindow());
        }
        else
        {
            // Now create a standalone window and renderer.
            native = input->GetHandler<Window>();
            native->Init(settings.title.c_str(), settings.width, settings.height, false, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            renderer = new Renderer(native);
        }
    }

    void EditorWindow::Init(InputController* controller, ResourceController* resourceController)
    {
        if (input != nullptr)
        {
            delete input;
        }
        input = new InputContext();
        controller->AddContext(Utilities::Format("EditorWindow {0}", this), input);
        NeuronGUI::Init(input, resourceController);
        Recreate();
    }

    Window* EditorWindow::GetNativeWindow()
    {
        if (parent != nullptr)
        {
            return parent->GetNativeWindow();
        }
        return native;
    }

    void EditorWindow::DockTo(EditorWindow* dock)
    {
        parent = dock;
        childID = parent->children.size();
        parent->children.push_back(this);
        Recreate();
    }

    void EditorWindow::Undock()
    {
        if (parent != nullptr)
        {
            auto itr = parent->children.begin() + childID;
            parent->children.erase(itr);
            // Update IDs of remaining children
            for (; itr != parent->children.end(); itr++)
            {
                (*itr)->childID--;
            }
            Recreate();
        }
    }

    void EditorWindow::SetTitle(string title)
    {
        settings.title = title;
        if (native != nullptr)
        {
            native->SetTitle(title);
        }
    }

    EditorWindow* EditorWindow::GetParent()
    {
        return parent;
    }

    void EditorWindow::Refresh()
    {
        // Clear the entire viewport
        SDL_RenderClear(renderer->GetRendererSDL());
        // Reset state
        Begin();

        // Window GUI
        /// TODO

        // Immediate-mode GUI i/o
        OnGUI();

        // Render GUI
        renderer->SetDrawColor(backgroundColor);
        SDL_RenderPresent(renderer->GetRendererSDL());

        // Set mouse cursor
        if (textFieldHovered)
        {
            MouseCursor::Set(SDL_SYSTEM_CURSOR_IBEAM);
        }
        else
        {
            MouseCursor::Set(SDL_SYSTEM_CURSOR_ARROW);
        }
    }

}
