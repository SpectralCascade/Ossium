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
        
        Renderer* renderer = GetService<Renderer>();
        Vector2 renderDimensions = Vector2(renderer->GetWidth(), renderer->GetHeight());

        BoxLayout* parent = entity->GetAncestor<BoxLayout>();
        if (parent != nullptr)
        {
            // This is the relative box, including padding
            Vector2 diff = (parent->anchorMinCached - parent->anchorMinCached);
            //                Start with total anchor |  Add this anchor   | Add padding
            anchorMinCached = parent->anchorMinCached + (diff * anchorMin) + (diff * parent->paddingMin);
            //                Ditto                   |  Sub this anchor   | Sub padding
            anchorMaxCached = parent->anchorMaxCached - (diff * anchorMax) - (diff * parent->paddingMax);
        }
        else
        {
            anchorMinCached = anchorMin;
            anchorMaxCached = anchorMax;
        }
        Vector2 minPos = anchorMinCached * renderDimensions;
        // Position at origin, based upon the absolute anchor positions.
        t->SetWorldPosition(minPos + (origin * ((anchorMaxCached * renderDimensions) - minPos)));
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
        return anchorMinCached * renderDimensions;
    }
    Vector2 BoxLayout::GetAnchorMaxPosition()
    {
        Renderer* renderer = GetService<Renderer>();
        Vector2 renderDimensions = Vector2(renderer->GetWidth(), renderer->GetHeight());
        return anchorMaxCached * renderDimensions;
    }

    Vector2 BoxLayout::GetDimensions()
    {
        Renderer* renderer = GetService<Renderer>();
        Vector2 renderDimensions = Vector2(renderer->GetWidth(), renderer->GetHeight());
        return (anchorMaxCached * renderDimensions) - (anchorMinCached * renderDimensions);
    }

    Vector2 BoxLayout::GetInnerDimensions()
    {
        Vector2 diff = GetDimensions();
        return diff - ((diff * paddingMin) + (diff * paddingMax));
    }

}
