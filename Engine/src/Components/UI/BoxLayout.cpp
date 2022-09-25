#include "BoxLayout.h"

namespace Ossium
{

    REGISTER_COMPONENT(BoxLayout);

    void BoxLayout::OnLoadFinish()
    {
        LayoutComponent::OnLoadFinish();
    }

    void BoxLayout::LayoutRefresh()
    {
        Transform* t = GetTransform();

        // BoxLayout overrides the transform, position is based on the layout and render dimensions.
        t->SetRelativeToParent(false);
        
        Renderer* renderer = GetService<Renderer>();
        Vector2 renderDimensions = Vector2(renderer->GetWidth(), renderer->GetHeight());

        BoxLayout* parent = entity->GetAncestor<BoxLayout>();
        if (parent != nullptr)
        {
            // This is the relative box, including padding
            Vector2 diff = (parent->anchorMaxCached - parent->anchorMinCached);
            //                Start with total anchor |  Add this anchor   | TODO: Add padding
            anchorMinCached = parent->anchorMinCached + Vector2(diff.x * anchorMin.x, diff.y * anchorMin.y);// + (diff * parent->paddingMin);
            //                Ditto                   |  Add this anchor   | TODO: Sub padding
            anchorMaxCached = parent->anchorMinCached + Vector2(diff.x * anchorMax.x, diff.y * anchorMax.y);// - (diff * parent->paddingMax);
        }
        else
        {
            anchorMinCached = anchorMin;
            anchorMaxCached = anchorMax;
        }
        Vector2 minPos = Vector2(anchorMinCached.x * renderDimensions.x, anchorMinCached.y * renderDimensions.y);
        // Position at origin, based upon the absolute anchor positions.
        Vector2 maxPos = (Vector2(anchorMaxCached.x * renderDimensions.x, anchorMaxCached.y * renderDimensions.y) - minPos);
        t->SetWorldPosition(minPos + Vector2(origin.x * maxPos.x, origin.y * maxPos.y));
    }


    Vector2 BoxLayout::GetCumulativeAnchorMin()
    {
        return anchorMinCached;
    }
    Vector2 BoxLayout::GetCumulativeAnchorMax()
    {
        return anchorMaxCached;
    }

    Vector2 BoxLayout::GetAnchorMinPosition()
    {
        Renderer* renderer = GetService<Renderer>();
        Vector2 renderDimensions = Vector2(renderer->GetWidth(), renderer->GetHeight());
        return Vector2(anchorMinCached.x * renderDimensions.x, anchorMinCached.y * renderDimensions.y);
    }
    Vector2 BoxLayout::GetAnchorMaxPosition()
    {
        Renderer* renderer = GetService<Renderer>();
        Vector2 renderDimensions = Vector2(renderer->GetWidth(), renderer->GetHeight());
        return Vector2(anchorMaxCached.x * renderDimensions.x, anchorMaxCached.y * renderDimensions.y);
    }

    Vector2 BoxLayout::GetDimensions()
    {
        Renderer* renderer = GetService<Renderer>();
        Vector2 renderDimensions = Vector2(renderer->GetWidth(), renderer->GetHeight());
        return Vector2(anchorMaxCached.x * renderDimensions.x, anchorMaxCached.y * renderDimensions.y)
            - Vector2(anchorMinCached.x * renderDimensions.x, anchorMinCached.y * renderDimensions.y);
    }

    Vector2 BoxLayout::GetInnerDimensions()
    {
        Vector2 diff = GetDimensions();
        return diff - (
            Vector2(diff.x * paddingMin.x, diff.y * paddingMin.y) +
            Vector2(diff.x * paddingMax.x, diff.y * paddingMax.y)
        );
    }

    bool BoxLayout::Contains(Vector2 worldPoint, bool innerDimensions)
    {
        Vector2 dimensions = innerDimensions ? GetInnerDimensions() : GetDimensions();
        Vector2 pos = GetTransform()->GetWorldPosition();
        Rect boxRect = Rect(pos.x - (dimensions.x / 2), pos.y - (dimensions.y / 2), dimensions.x, dimensions.y);
        return boxRect.Contains(worldPoint);
    }

    bool BoxLayout::Contains(Vector2 p)
    {
        return Contains(p, true);
    }

}
