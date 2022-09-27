#include "renderview.h"

namespace Ossium
{

    RenderView::RenderView(bgfx::ViewId id, SDL_Rect rect, bgfx::FrameBufferHandle target, std::string name)
    {
        this->id = id;
        SetDebugName(name);
        SetRenderTarget(target);
        SetRenderRect(rect);
    }

    void RenderView::SetRenderTarget(bgfx::FrameBufferHandle target)
    {
        this->target = target;
        bgfx::setViewFrameBuffer(id, target);
    }

    bgfx::FrameBufferHandle RenderView::GetRenderTarget()
    {
        return target;        
    }

    void RenderView::SetRenderRect(SDL_Rect rect)
    {
        this->rect = rect;
        bgfx::setViewRect(id, (uint16_t)rect.x, (uint16_t)rect.y, (uint16_t)rect.w, (uint16_t)rect.h);
    }

    SDL_Rect RenderView::GetRenderRect()
    {
        return rect;
    }

    bgfx::ViewId RenderView::GetID()
    {
        return id;
    }

    void RenderView::SetDebugName(std::string name)
    {
        this->name = name;
        bgfx::setViewName(id, this->name.c_str());
    }

    std::string RenderView::GetDebugName()
    {
        return name;
    }

    RenderViewPool::RenderViewPool(size_t prealloc)
    {
        views.reserve(prealloc);
    }

    bgfx::ViewId RenderViewPool::Create(SDL_Rect rect, bgfx::FrameBufferHandle target, std::string name) {
        if (head < views.size())
        {
            // Use head as id
            views[head] = RenderView(head, rect, target, name);
        }
        else
        {
            views.push_back(RenderView(head, rect, target, name));
        }
        return head++;
    }

    void RenderViewPool::Free(bgfx::ViewId id)
    {
        if (id >= head)
        {
            throw ("RenderView id " + Utilities::ToString((int)id) + " out of range").c_str();
        }
        if (bgfx::isValid(views[id].GetRenderTarget()))
        {
            // Assume destruction of frame buffer target happens elsewhere
            views[id].SetRenderTarget(BGFX_INVALID_HANDLE);
        }
        if (id == head - 1)
        {
            head--;
            // Cycle back through unused ids; not smart but simple.
            while (!ids.empty() && head - 1 == ids.top())
            {
                ids.pop();
                head--;
            }
        }
        else
        {
            ids.push(id);
        }
    }
    
    RenderView& RenderViewPool::Get(bgfx::ViewId id)
    {
        if (id >= head)
        {
            throw ("RenderView id " + Utilities::ToString((int)id) + " out of range").c_str();
        }
        return views[id];
    }

}
