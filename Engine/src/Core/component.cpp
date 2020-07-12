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
#include "component.h"

namespace Ossium
{

    REGISTER_ABSTRACT_COMPONENT(Component, BaseComponent);

    ///
    /// Component
    ///

    Transform* Component::GetTransform()
    {
        return entity->AddComponentOnce<Transform>();
    }

    ///
    /// GraphicComponent
    ///

    void GraphicComponent::SetRenderLayer(int layer)
    {
        if (GetService<Renderer>() != nullptr)
        {
            if (renderLayer >= 0)
            {
                GetService<Renderer>()->Unregister(this, renderLayer);
            }
            renderLayer = GetService<Renderer>()->Register(this, layer);
        }
    }

    int GraphicComponent::GetRenderLayer()
    {
        return renderLayer;
    }

    GraphicComponent::GraphicComponent()
    {
    }

    GraphicComponent::~GraphicComponent()
    {
    }

    void GraphicComponent::OnCreate()
    {
        Component::OnCreate();
    }

    void GraphicComponent::OnLoadStart()
    {
        Component::OnLoadStart();
        if (GetService<Renderer>() != nullptr && renderLayer >= 0)
        {
            GetService<Renderer>()->Unregister(this, renderLayer);
            renderLayer = -1;
        }
    }

    void GraphicComponent::OnLoadFinish()
    {
        Component::OnLoadFinish();
        if (GetService<Renderer>() != nullptr)
        {
            if (renderLayer < 0)
            {
                renderLayer = 0;
            }
            renderLayer = GetService<Renderer>()->Register(this, renderLayer);
        }
    }

    void GraphicComponent::OnDestroy()
    {
        Component::OnDestroy();
        if (GetService<Renderer>() != nullptr && renderLayer >= 0)
        {
            GetService<Renderer>()->Unregister(this, renderLayer);
            renderLayer = -1;
        }
    }

    void GraphicComponent::OnClone(BaseComponent* src)
    {
    }

    void GraphicComponent::Update()
    {
    }

}
