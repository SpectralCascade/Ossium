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
#ifndef COMPONENT_H
#define COMPONENT_H

#include "transform.h"

namespace Ossium
{

    // General purpose component that is not rendered
    class OSSIUM_EDL Component : public BaseComponent
    {
    public:
        DECLARE_ABSTRACT_COMPONENT(BaseComponent, Component);

        Transform* GetTransform();

    };

    // General purpose component that a Canvas can render.
    class OSSIUM_EDL GraphicComponent : public Graphic, public Component
    {
    protected:
        DECLARE_ABSTRACT_COMPONENT(Component, GraphicComponent);

        virtual void Draw(RenderInput* pass) = 0;

    };

    // General purpose component that a Camera can render.
    class OSSIUM_EDL RenderComponent : public Renderable, public Component
    {
    protected:
        DECLARE_ABSTRACT_COMPONENT(Component, RenderComponent);

        virtual void Render(RenderInput* pass, const Matrix<4, 4>& view, const Matrix<4, 4>& proj) = 0;

    };

}

#endif // COMPONENT_H
