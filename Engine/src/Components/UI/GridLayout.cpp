#include "GridLayout.h"

namespace Ossium
{

    REGISTER_COMPONENT(GridLayout);

    void GridLayout::OnDestroy()
    {
        LayoutComponent::OnDestroy();
    }

    void GridLayout::OnLoadFinish()
    {
        LayoutComponent::OnLoadFinish();
    }

    BoxLayout* GridLayout::GetCellElement(unsigned int col, unsigned int row)
    {
#ifdef OSSIUM_DEBUG
        // This error message doesn't account for if the cols or rows are manually set but the layout is not refreshed.
        std::string msg = Utilities::Format("GridLayout::GetCellElement({0}, {1}) error, argument(s) out of range! GridLayout cols = {2}, rows = {3}", col, row, cols, rows);
        DEBUG_ASSERT(
            col < cellElements.size() && row < cellElements[col].size(),
            msg
        );
#endif
        return cellElements[col][row];
    }

    BoxLayout* GridLayout::GetCellElementAt(Vector2 worldPos, unsigned int* outcol, unsigned int* outrow)
    {
        // Obtain grid dimensions
        Rect dimensions(GetTransform()->GetWorldPosition(), Vector2::Zero);
        BoxLayout* mainLayout = entity->GetComponent<BoxLayout>();
        if (mainLayout == nullptr)
        {
            mainLayout = entity->GetAncestor<BoxLayout>();
        }
        if (mainLayout != nullptr)
        {
            dimensions = Rect(Vector2(dimensions.x, dimensions.y), mainLayout->GetDimensions());
        }
        else
        {
            // Use renderer dimensions.
            SDL_Rect vrect = entity->GetService<Renderer>()->GetViewportRect();
            dimensions.w = vrect.w;
            dimensions.h = vrect.h;
        }

        // Position for rect needs to be in the upper-left, not centre.
        dimensions.x -= dimensions.w / 2.0f;
        dimensions.y -= dimensions.h / 2.0f;

        // Now determine which cell the worldPos intersects, if any.
        if (dimensions.Contains(worldPos))
        {
            float colWidth = dimensions.w / (float)cols;
            float rowHeight = dimensions.h / (float)rows;

            // Map position to the grid, using integer truncation.
            unsigned int col = (unsigned int)((worldPos.x - dimensions.x) / colWidth);
            unsigned int row = (unsigned int)((worldPos.y - dimensions.y) / rowHeight);

            if (outcol != nullptr)
            {
                *outcol = col;
            }
            if (outrow != nullptr)
            {
                *outrow = row;
            }

            return GetCellElement(col, row);
        }
        return nullptr;
    }

    void GridLayout::LayoutRefresh()
    {
        // Destroy pre-existing cells that won't be reused.
        for (unsigned int i = cols, counti = cellElements.size(); i < counti; i++)
        {
            for (unsigned int j = rows, countj = cellElements[i].size(); j < countj; j++)
            {
                Log.Debug("Destroying unused rows and columns!");
                cellElements[i][j]->GetEntity()->Destroy(
#ifdef OSSIUM_EDITOR
                    true
#endif
                );
            }
        }

        auto children = entity->GetComponentsInChildren<Transform>();
        unsigned int totalChildren = children.size();

        cellElements.clear();
        cellElements.resize(cols, {});
        
        // Now create missing cells per column and update the layout configuration.
        for (unsigned int i = 0, counti = (unsigned int)std::max((int)cols, (int)totalChildren); i < counti; i++)
        {
            if (i >= cols)
            {
                // Destroy unused column entities
                if (i < totalChildren)
                {
                    children[i]->GetEntity()->Destroy(
#ifdef OSSIUM_EDITOR
                        true
#endif
                    );
                }
                continue;
            }

            cellElements[i].resize(rows, nullptr);

            if (i >= totalChildren)
            {
                // Create missing column entity.
                children.push_back(entity->CreateChild()->AddComponentOnce<Transform>());
            }

            auto cells = children[i]->GetEntity()->GetComponentsInChildren<BoxLayout>();

            // How many cells are already created in this column?
            unsigned int cellCount = cells.size();
            for (unsigned int j = 0, countj = (unsigned int)std::max((int)rows, (int)cellCount); j < countj; j++)
            {
                if (j >= rows && j < cells.size())
                {
                    // Destroy unused row entities
                    cells[j]->GetEntity()->Destroy(
#ifdef OSSIUM_EDITOR
                        true
#endif
                    );
                    continue;
                }
                else if (j >= cellCount)
                {
                    // Create missing cell for this row.
                    cells.push_back(children[i]->GetEntity()->CreateChild()->AddComponentOnce<BoxLayout>());
                }

                // Override BoxLayout anchors, which will in turn update the transform position.
                cells[j]->anchorMin = Vector2((float)i / (float)cols, (float)j / (float)rows);
                cells[j]->anchorMax = Vector2((float)(i + 1) / (float)cols, (float)(j + 1) / (float)rows);

                cellElements[i][j] = cells[j];
            }
        }
    }

}
