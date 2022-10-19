#include "renderview.h"
#include "logging.h"
#include "rendertarget.h"

namespace Ossium
{

    RenderView::RenderView(RenderViewPool* pool, bgfx::ViewId id, SDL_Rect viewport, RenderTarget* target, std::string name)
    {
        Log.Info("Creating render view {0} \"{1}\" with target: {2} (w: {3}, h: {4})", id, name, target->GetFrameBuffer().idx, target->GetWidth(), target->GetHeight());
        this->pool = pool;
        this->id = id;
        SetDebugName(name);
        SetRenderTarget(target);
        SetViewport(viewport);
    }

    void RenderView::SetRenderTarget(RenderTarget* target)
    {
        this->target = target;
        bgfx::setViewFrameBuffer(id, target->GetFrameBuffer());
    }

    RenderTarget* RenderView::GetRenderTarget()
    {
        return target;
    }

    void RenderView::SetViewport(SDL_Rect rect)
    {
        this->viewport = rect;
        bgfx::setViewRect(id, (uint16_t)rect.x, (uint16_t)rect.y, (uint16_t)rect.w, (uint16_t)rect.h);
    }

    SDL_Rect RenderView::GetViewport()
    {
        return viewport;
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

    RenderView* RenderViewPool::Create(SDL_Rect viewport, RenderTarget* target, std::string name) {
        RenderView* created = new RenderView(this, views.size(), viewport, target, name);
        views.push_back(created);
        return created;
    }

    void RenderViewPool::Free(bgfx::ViewId id)
    {
        DEBUG_ASSERT(id < views.size() && id >= 0, "RenderView id " + Utilities::ToString((int)id) + " out of range");
        
        // Destroy the view
        delete views[id];
        
        // Shift pointers and ids down to maintain contiguous view order
        for (unsigned int i = id, counti = views.size() - 1; i < counti; i++)
        {
            views[i] = views[i + 1];
            // TODO refresh view?
            views[i]->id = i;
        }

        // Remove the (now unused) last element
        views.pop_back();
    }

    void RenderViewPool::Free(RenderView* view)
    {
        Free(view->id);
    }
    
    RenderView* RenderViewPool::Get(bgfx::ViewId id)
    {
        DEBUG_ASSERT(id < views.size() && id >= 0, "RenderView id " + Utilities::ToString((int)id) + " out of range");
        return views[id];
    }

}
