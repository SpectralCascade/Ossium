#include "layout_diagram.h"

namespace Ossium::Editor
{

    EditorWindow* LayoutDiagram::mouseHovered = nullptr;

    void LayoutDiagram::OnGUI()
    {
        // Note: this assumes the current mode is breadth-first
        vector<Node<EditorRect>*>& layout = target != nullptr ? target->GetLayout()->GetFlatTree() : GetEditorLayout()->GetLayout()->GetFlatTree();

        int yspacing = 40;
        int nodeSize = 16;
        SDL_Color leafColor = Colors::GREEN;
        SDL_Color parentColor = Colors::BLUE;
        SDL_Color lineColor = Colors::BLACK;

        Node<EditorRect>* currentParent = nullptr;
        vector<int> parentX;
        vector<int> nextParentX;
        int y = 30;
        int x = nodeSize;
        int halfNode = (nodeSize / 2);
        int parentIndex = 0;

        for (auto itr = layout.begin(); itr < layout.end(); itr++)
        {
            auto n = *itr;
            if (n->parent != nullptr)
            {
                if (currentParent == nullptr || currentParent->depth != n->parent->depth)
                {
                    // Increment Y
                    y += yspacing;
                    // Set x pos to left hand side of the screen
                    x = 10;
                    parentX = nextParentX;
                    nextParentX.clear();
                    //Log.Debug("Next parentX cleared, level {0}", n->depth - 1);

                    parentIndex = 0;
                    //Log.Debug("new level, parentIndex = 0");
                }
                else
                {
                    parentIndex += (int)(currentParent != n->parent);
                    if (currentParent != n->parent)
                    {
                        //Log.Debug("parentIndex incremented");
                    }
                }
            }

            // Draw the node
            Rect(x, y, nodeSize, nodeSize).DrawFilled(*renderer, n->children.empty() ? leafColor : parentColor);
            if (n->data.window != nullptr && n->data.window == mouseHovered)
            {
                Rect(x - 1, y - 1, nodeSize + 2, nodeSize + 2).Draw(*renderer, Colors::WHITE);
            }
            if (!parentX.empty())
            {
                // Draw line between node and parent
                Line line = Line(Vector2(x + halfNode, y + halfNode), Vector2(parentX[parentIndex] + halfNode, (y - yspacing) + halfNode));
                line.Draw(*renderer, lineColor);
                //Log.Debug("Drawing line from {0} to {1} [child index = {2} - parent index = {3} of parent x {4}]", line.a, line.b, n->childIndex, parentIndex, parentX);
            }
            if (!n->children.empty())
            {
                nextParentX.push_back(x);
            }
            x += nodeSize * 2;

            currentParent = n->parent;
            //Log.Debug("parentX = {0} at level {1}, nextParentX = {2} (node: {3} parent: {4})", parentX, n->depth, nextParentX, n, n->parent);

        }

    }

}
