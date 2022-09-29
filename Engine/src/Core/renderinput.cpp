#include "renderinput.h"

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

}
