#include "editorlayout.h"
#include "editorwindow.h"
#include "editorcontroller.h"

using namespace std;

namespace Ossium::Editor
{


    //
    // EditorLayout
    //

    EditorLayout::EditorLayout(EditorController* controller, string title, int w, int h)
    {
        this->controller = controller;
        input = controller->GetInput();
        resources = controller->GetResources();

        input->AddContext(Utilities::Format("EditorLayout {0}", this), &windowContext);
        native = new Window(title.c_str(), w, h, false, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | (w < 0 || h < 0 ? SDL_WINDOW_MAXIMIZED : 0));

        // TODO: make this a method of the Window class
        SDL_SetWindowMinimumSize(native->GetWindowSDL(), (int)MIN_DIMENSIONS.x, (int)MIN_DIMENSIONS.y);

        renderer = new Renderer(native);
        renderBuffer = SDL_CreateTexture(renderer->GetRendererSDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w <= 0 ? 1 : w, h <= 0 ? 1 : h);

        // Use breadth-first traversal to get the flat tree
        layout.SetFlatTreeBuildMode(false);

        // Handle window resize
        native->OnSizeChanged += [&] (Window& window) {
            if (renderBuffer != NULL)
            {
                SDL_DestroyTexture(renderBuffer);
                renderBuffer = NULL;
            }
            renderBuffer = SDL_CreateTexture(renderer->GetRendererSDL(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window.GetWidth(), window.GetHeight());

            // Update the root node
            auto roots = layout.GetRoots();
            if (!roots.empty())
            {
                roots[0]->data.w = window.GetWidth();
                roots[0]->data.h = window.GetHeight();
            }

            updateViewports = true;

            return ActionOutcome::Ignore;
        };

        native->OnMouseEnter += [&] (Window& window) {
            for (auto node : layout.GetFlatTree()) {
                if (node->data.window != nullptr)
                {
                    node->data.window->input->SetActive(true);
                }
            }
        };

        native->OnMouseLeave += [&] (Window& window) {
            for (auto node : layout.GetFlatTree()) {
                if (node->data.window != nullptr)
                {
                    node->data.window->input->SetActive(false);
                }
            }
        };

        native->OnCloseButton += [&] (Window& window) {
            GetEditorController()->RemoveLayout(this);
        };

        // Provide access to basic services like the renderer and resources.
        services = new ServicesProvider(resources, renderer);

    }

    EditorLayout::~EditorLayout()
    {
        // Manually run through the tree, deleting all windows.
        for (auto node : layout.GetFlatTree())
        {
            if (node->data.window != nullptr)
            {
                delete node->data.window;
            }
        }
        layout.Clear();
        if (input != nullptr)
        {
            input->RemoveContext(Utilities::Format("EditorLayout {0}", this));
        }
        if (renderBuffer != NULL)
        {
            SDL_DestroyTexture(renderBuffer);
            renderBuffer = NULL;
        }
        if (renderer != nullptr)
        {
            delete renderer;
            renderer = nullptr;
        }
        if (native != nullptr)
        {
            /// NOTE TO FUTURE ME: this seems to set off the debugger?
            delete native;
            native = nullptr;
        }
        delete services;
    }

    ServicesProvider* EditorLayout::GetServices()
    {
        return services;
    }

    void EditorLayout::HandleEvent(SDL_Event& e)
    {
        if (native != nullptr)
        {
            native->HandleEvent(e);
        }
    }

    Tree<EditorRect>* EditorLayout::GetLayout()
    {
        return &layout;
    }

    void EditorLayout::Update(bool forceUpdate)
    {

        // Setup rendering to texture
        SDL_Renderer* render = renderer->GetRendererSDL();
        SDL_SetRenderTarget(render, renderBuffer);

        // These are used for computing viewport sizes
        Node<EditorRect>* currentGroup = nullptr;
        int combinedSize = 0;

        // Breadth-first layout iteration; just in case the flat tree is updated during this process (e.g. another editor window is added), only iterate up to the current end point.
        vector<Node<EditorRect>*>& flatTree = layout.GetFlatTree();
        auto endItr = flatTree.end();
        auto length = flatTree.size();

        // First stage - update the viewports (but only if necessary).
        // Optimiser: keep track of nodes that need updating and only update those sections of the layout tree?
        if (updateViewports || forceUpdate)
        {
            for (auto itr = flatTree.begin(); itr < endItr; itr++)
            {
                Node<EditorRect>* node = *itr;

                if (node->parent != nullptr)
                {
                    bool isRow = node->depth % 2 == layoutRow;

                    // Have we just iterated over a grouping of nodes (or the very last node)?
                    bool lastNode = itr == flatTree.begin() + (length - 1);
                    if ((currentGroup != node->parent || lastNode) && currentGroup != nullptr)
                    {
                        if (lastNode && currentGroup == node->parent)
                        {
                            combinedSize += isRow ? node->data.w : node->data.h;
                        }
                        //Log.Debug("\nUpdating group [{0}] {1}", currentGroup, currentGroup->data);

                        bool currentRow = currentGroup->depth % 2 == layoutColumn;
                        float scaleFactor = (currentRow ? currentGroup->data.w : currentGroup->data.h) / (float)combinedSize;

                        //Log.Debug("Current group is a {0}, combined size = {1}, scale factor = {2}", currentRow ? "row" : "column", combinedSize, scaleFactor);

                        // Update the entire group of rectangles now we know their total combined width/height
                        if (currentRow)
                        {
                            float nextPos = currentGroup->data.x;
                            for (Node<EditorRect>* child : currentGroup->children)
                            {
                                float change = (float)child->data.w * scaleFactor;
                                child->data.w = round(change);
                                child->data.x = round(nextPos);
                                child->data.y = currentGroup->data.y;
                                child->data.h = currentGroup->data.h;
                                if (child->data.window != nullptr)
                                {
                                    child->data.window->viewport = child->data.SDL();
                                }
                                nextPos += change;
                            }
                        }
                        else
                        {
                            float nextPos = currentGroup->data.y;
                            for (Node<EditorRect>* child : currentGroup->children)
                            {
                                float change = (float)child->data.h * scaleFactor;
                                child->data.h = round(change);
                                child->data.y = round(nextPos);
                                child->data.x = currentGroup->data.x;
                                child->data.w = currentGroup->data.w;
                                if (child->data.window != nullptr)
                                {
                                    child->data.window->viewport = child->data.SDL();
                                }
                                nextPos += change;
                            }
                        }

                        combinedSize = isRow ? node->data.w : node->data.h;
                    }
                    else
                    {
                        combinedSize += isRow ? node->data.w : node->data.h;
                    }
                }
                else
                {
                    // Must be a root node (for this use case, there will only be one root node).
                    // Always matches the native window rect.
                    node->data.w = native->GetWidth();
                    node->data.h = native->GetHeight();
                    node->data.x = 0;
                    node->data.y = 0;
                }
                currentGroup = node->parent;

                if (node->data.window != nullptr)
                {
                    node->data.window->viewport = node->data.SDL();
                }
            }
        }
        bool didUpdateViewports = updateViewports;
        updateViewports = false;

        // Second stage - logic and rendering.
        for (auto itr = flatTree.begin(); itr != endItr; itr++)
        {
            Node<EditorRect>* node = *itr;

            // Now update the editor window, if this node is not a stem
            if (node->data.window != nullptr)
            {
                node->data.window->Update(didUpdateViewports || forceUpdate);
            }
        }

        renderer->SetViewportRect({0, 0, native->GetWidth(), native->GetHeight()});

        // Change target back to the window and render the texture
        SDL_SetRenderTarget(render, NULL);
        SDL_RenderClear(render);
        SDL_RenderCopy(render, renderBuffer, NULL, NULL);
        SDL_RenderPresent(renderer->GetRendererSDL());

        // Only remove windows when everything has finished updating.
        if (!toRemove.empty())
        {
            updateViewports = true;
            for (auto window : toRemove)
            {
                Remove(window);
            }
            toRemove.clear();
        }
    }

    Window* EditorLayout::GetNativeWindow()
    {
        return native;
    }

    bool EditorLayout::Insert(EditorWindow* source, EditorWindow* dest, DockingMode mode)
    {
        if (dest->node == nullptr)
        {
            Log.Error("Cannot dock editor window to window that has a NULL layout node!");
            return false;
        }

        EditorRect sourceRect = EditorRect(source->viewport);
        sourceRect.window = source;
        EditorRect destRect = dest->node->data;
        switch (mode)
        {
            case TOP:
            case BOTTOM:
                if (dest->node->depth == 0)
                {
                    // Make the current destination root
                    dest->node->data.window = nullptr;
                    // Move the destination node down a level
                    dest->node = layout.Insert(destRect, dest->node);
                    destRect.window->node = dest->node;
                    layoutRow = 0;
                    layoutColumn = 1;
                }

                // TODO: proper dimension checks, return false if there's not enough space
                destRect.h = max(MIN_DIMENSIONS.y, destRect.h - sourceRect.h);

                // Warning: rect positioning and resizing assumes the source rect has already been resized to fit!
                if (mode == DockingMode::BOTTOM)
                {
                    sourceRect.y = destRect.y + destRect.h;
                }
                else
                {
                    sourceRect.y = destRect.y;
                    destRect.y += sourceRect.h;
                }
                sourceRect.w = destRect.w;
                sourceRect.x = destRect.x;

                if (dest->node->depth % 2 == layoutColumn)
                {
                    source->node = layout.Insert(sourceRect, dest->node->parent, mode == DockingMode::BOTTOM ? dest->node->childIndex + 1 : dest->node->childIndex);
                }
                else
                {
                    dest->node->data.window = nullptr;
                    auto parent = dest->node;
                    dest->node = layout.Insert(destRect, parent);
                    source->node = layout.Insert(sourceRect, parent, mode == DockingMode::BOTTOM);
                }
                break;
            case LEFT:
            case RIGHT:
                // Same as above indent block, but for inserting as a column rather than as a row.
                if (dest->node->depth == 0)
                {
                    // Make the current destination root
                    dest->node->data.window = nullptr;
                    // Move the destination node down a level
                    dest->node = layout.Insert(destRect, dest->node);
                    destRect.window->node = dest->node;
                    layoutRow = 1;
                    layoutColumn = 0;
                }

                // TODO: proper dimension checks, return false if there's not enough space
                destRect.w = max(MIN_DIMENSIONS.x, destRect.w - sourceRect.w);

                // Warning: rect positioning and resizing assumes the source rect has already been resized to fit!
                if (mode == DockingMode::RIGHT)
                {
                    sourceRect.x = destRect.x + destRect.w;
                }
                else
                {
                    sourceRect.x = destRect.x;
                    destRect.x += sourceRect.w;
                }
                sourceRect.h = destRect.h;
                sourceRect.y = destRect.y;

                if (dest->node->depth % 2 == layoutRow)
                {
                    source->node = layout.Insert(sourceRect, dest->node->parent, mode == DockingMode::RIGHT ? dest->node->childIndex + 1 : dest->node->childIndex);
                }
                else
                {
                    dest->node->data.window = nullptr;
                    auto parent = dest->node;
                    dest->node = layout.Insert(destRect, parent);
                    source->node = layout.Insert(sourceRect, parent, mode == DockingMode::RIGHT);
                }

                break;
            case TOP_ADJACENT:
            case BOTTOM_ADJACENT:
                break;
            case LEFT_ADJACENT:
            case RIGHT_ADJACENT:
                break;
            default:
                // Invalid docking mode
                break;
        }
        dest->node->data = destRect;
        source->node->data = sourceRect;
        //Log.Debug("Source window viewport = {0}, dest window viewport = {1}", sourceRect, destRect);
        source->renderer = renderer;
        source->viewport = sourceRect.SDL();
        dest->viewport = destRect.SDL();
        source->native = this;

        updateViewports = true;
        return true;
    }

    bool EditorLayout::Remove(EditorWindow* source)
    {
        // First check for a single sibling node
        Node<EditorRect>* sibling = nullptr;
        if (source != nullptr && source->node != nullptr && source->node->parent != nullptr && source->node->parent->children.size() == 2)
        {
            sibling = source->node->parent->children[(unsigned int)(!source->node->childIndex)];
        }
        // Now the sibling is determined, the source node can be safely removed
        if (!layout.Remove(source->node))
        {
            Log.Error("Failed to remove EditorWindow node!");
        }

        if (sibling != nullptr)
        {
            // When removing a node that only has one other sibling, the tree needs to be normalised
            // i.e. all nodes must have at least one sibling (except the root node), and every leaf must have a valid editor window instance.
            // There are 2 main cases in this scenario; one is that the sibling node is a leaf, and one where it is not.

            if (sibling->data.window != nullptr)
            {
                // Simple case, sibling is a leaf so just move the sibling up a level
                sibling->parent->data.window = sibling->data.window;
                sibling->data.window->node = sibling->parent;
                layout.Remove(sibling);
                //Log.Debug("Removed sibling which was a leaf, moved it up.");
            }
            else
            {
                // Complex case, ditch the sibling but keep it's children while maintaining tree normalisation.
                if (sibling->depth == 1)
                {
                    // If the parent is the root node, then the root node becomes the sibling and the layout levels are switched.
                    sibling->parent->data.window = sibling->data.window;
                    for (auto child : sibling->children)
                    {
                        child->SetParent(sibling->parent);
                    }
                    layout.Remove(sibling);
                    //Log.Debug("Removed sibling, made it root.");
                    // Flip the layout levels - rows become columns, columns become rows.
                    layoutRow = !layoutRow;
                    layoutColumn = !layoutColumn;
                }
                else
                {
                    // In this scenario, move all the children of the sibling node 2 levels up while maintaining the order.
                    int insertIndex = sibling->parent->childIndex;
                    //Log.Debug("Removing null sibling at depth {0} child index {1}, moved it's children up 2 levels.", sibling->depth, insertIndex);
                    vector<Node<EditorRect>*> children = sibling->children;
                    for (auto child : children)
                    {
                        child->SetParent(sibling->parent->parent, insertIndex);
                        //Log.Info("Inserted child at {0} (new depth = {1}, true child index = {2})", insertIndex, child->depth, child->childIndex);
                        insertIndex++;
                    }
                    // Remove both the sibling node and it's parent
                    layout.Remove(sibling->parent);
                }
            }
            updateViewports = true;
            //Log.Debug("Normalised tree after removing a window node");
        }
        if (source != nullptr)
        {
            Log.Debug("Destroying window \"{0}\"", source->title);
            delete source;
        }
        else
        {
            Log.Warning("Attempted to remove editor window that is already destroyed!");
        }
        return true;
    }

    Node<EditorRect>* EditorLayout::FindSibling(Node<EditorRect>* node, int dir, bool isRow)
    {
        if (node == nullptr)
        {
            // Base case. Can't do anything beyond the root node.
            return nullptr;
        }
        // Is this node a row or column as intended?
        if (node->parent != nullptr && node->depth % 2 == (isRow ? layoutRow : layoutColumn))
        {
            // If so, check for a sibling
            int index = node->childIndex + dir;
            if (index >= 0 && index < (int)node->parent->children.size())
            {
                return node->parent->children[index];
            }
        }
        // If not, move on up the tree.
        return FindSibling(node->parent, dir, isRow);
    }

    void EditorLayout::Resize(EditorWindow* window, Rect rect)
    {
        // First, normalise the rect
        rect = Rect(Utilities::Clamp(rect.x, 0.0f, (float)native->GetWidth()),
                    Utilities::Clamp(rect.y, 0.0f, (float)native->GetHeight()),
                    Utilities::Clamp(rect.w, MIN_DIMENSIONS.x, (float)native->GetWidth()),
                    Utilities::Clamp(rect.h, MIN_DIMENSIONS.y, (float)native->GetHeight())
        );

        Node<EditorRect>* node = window->node;

        bool isRow = node->depth % 2 == layoutRow;

        // Locate the 4 adjacent nodes
        Node<EditorRect>* top = FindSibling(node, -1, isRow);
        Node<EditorRect>* bottom = FindSibling(node, 1, isRow);
        Node<EditorRect>* left = FindSibling(node, -1, !isRow);
        Node<EditorRect>* right = FindSibling(node, 1, !isRow);

        float minFloat, maxFloat;

        // Now the fun begins
        if (rect.x != node->data.x)
        {
            maxFloat = node->data.xmax() - window->minDimensions.x;
            if (left != nullptr)
            {
                // TODO: If node is not a window, handle minimum size based on child windows.
                minFloat = left->data.x + (left->data.window != nullptr ? left->data.window->minDimensions.x : MIN_DIMENSIONS.x);
            }
            else
            {
                minFloat = node->data.window->minDimensions.x;
            }
            float xmax = node->data.xmax();
            // TODO: handle undefined behaviour (where minpos > maxpos).
            node->data.x = Utilities::Clamp(rect.x, minFloat, maxFloat);
            node->data.w = xmax - node->data.x;
            if (left != nullptr)
            {
                left->data.w = node->data.x - left->data.x;
            }
            updateViewports = true;
        }
        else if (rect.w != node->data.w)
        {
            minFloat = window->minDimensions.x;
            if (right != nullptr)
            {
                maxFloat = (right->data.xmax() - node->data.x) - (right->data.window != nullptr ? right->data.window->minDimensions.x : MIN_DIMENSIONS.x);
            }
            else
            {
                maxFloat = native->GetWidth() - node->data.x;
            }
            node->data.w = Utilities::Clamp(rect.w, minFloat, maxFloat);
            if (right != nullptr)
            {
                right->data.w = right->data.xmax() - node->data.xmax();
                right->data.x = node->data.xmax();
            }
            updateViewports = true;
        }

        if (rect.y != node->data.y)
        {
            maxFloat = node->data.ymax() - window->minDimensions.y;
            if (top != nullptr)
            {
                minFloat = top->data.y + (top->data.window != nullptr ? top->data.window->minDimensions.y : MIN_DIMENSIONS.y);
            }
            else
            {
                minFloat = window->minDimensions.y;
            }
            float ymax = node->data.ymax();
            node->data.y = Utilities::Clamp(rect.y, minFloat, maxFloat);
            node->data.h = ymax - node->data.y;
            if (top != nullptr)
            {
                top->data.h = node->data.y - top->data.y;
            }
            updateViewports = true;
        }
        else if (rect.h != node->data.h)
        {
            minFloat = window->minDimensions.y;
            if (bottom != nullptr)
            {
                maxFloat = (bottom->data.ymax() - node->data.y) - (bottom->data.window != nullptr ? bottom->data.window->minDimensions.y : MIN_DIMENSIONS.y);
            }
            else
            {
                maxFloat = native->GetHeight() - node->data.y;
            }
            node->data.h = Utilities::Clamp(rect.h, minFloat, maxFloat);
            if (bottom != nullptr)
            {
                bottom->data.h = bottom->data.ymax() - node->data.ymax();
                bottom->data.y = node->data.ymax();
            }
            updateViewports = true;
        }

    }

    void EditorLayout::RemovePostUpdate(EditorWindow* window)
    {
        toRemove.insert(window);
    }

    InputController* EditorLayout::GetInputController()
    {
        return input;
    }

    ResourceController* EditorLayout::GetResources()
    {
        return resources;
    }

    EditorController* EditorLayout::GetEditorController()
    {
        return controller;
    }

}
