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

    class OSSIUM_EDL Component : public BaseComponent, public GlobalServices
    {
    public:
        DECLARE_ABSTRACT_COMPONENT(Component);

        Transform* GetTransform();

    };

    struct OSSIUM_EDL GraphicComponentSchema : public Schema<GraphicComponentSchema, 1>
    {
    public:
        DECLARE_BASE_SCHEMA(GraphicComponentSchema, 1);

    protected:
        M(int, renderLayer) = -1;

    };

    class OSSIUM_EDL GraphicComponent : public Graphic, public Component, public GraphicComponentSchema
    {
    public:
        CONSTRUCT_SCHEMA(Component, GraphicComponentSchema);

        /// Attempts to set the rendering layer of this graphic component. Note that you probably shouldn't call this
        /// too frequently as it attempts a removal from one set and insertion into another set within the renderer.
        void SetRenderLayer(int layer);

        /// Returns the layer this component is being rendered on.
        int GetRenderLayer();

    protected:
        DECLARE_ABSTRACT_COMPONENT(GraphicComponent);

        virtual void Render(Renderer& renderer) = 0;

    private:
        using GraphicComponentSchema::renderLayer;

    };

}

#endif // COMPONENT_H
