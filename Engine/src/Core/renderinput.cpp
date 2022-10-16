#include "renderinput.h"
#include "renderview.h"

namespace Ossium
{

    bool RenderInput::IsRenderEnabled()
    {
        return true;
    }
    
    RenderView* RenderInput::GetRenderView()
    {
        return renderView;
    }

    Renderer* RenderInput::GetRenderer()
    {
        return renderer;
    }

    uint16_t RenderInput::GetID()
    {
        return renderView->GetID();
    }

    int RenderInput::GetViewMode()
    {
        return bgfx::ViewMode::Default;
    }

}
