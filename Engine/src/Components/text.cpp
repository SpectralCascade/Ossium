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
#include <string>
#include <algorithm>
#ifndef __ANDROID__
#include <filesystem>
#endif

#include "text.h"
#include "UI/BoxLayout.h"
#include "../Core/renderer.h"
#include "../Core/ecs.h"

namespace Ossium
{

    REGISTER_COMPONENT(Text);

    void Text::OnLoadFinish()
    {
        ParentType::OnLoadFinish();
        // Check if the path is valid
        if (
#ifndef __ANDROID__
            std::filesystem::exists(std::filesystem::path(font_guid))
#else
            true // TODO: replace std::filesystem exists() check
#endif
        ) {
            font = GetService<ResourceController>()->Get<Font>(font_guid, 72, 0, 0, 2048);
        }
        else
        {
            font = nullptr;
        }
        dirty = true;
    }

    void Text::Render(Renderer& renderer)
    {
        BoxLayout* boxLayout = entity->GetComponent<BoxLayout>();
        if (boxLayout != nullptr)
        {
            // TODO: account for padding?
            Vector2 dimensions = boxLayout->GetInnerDimensions();
            bounds = dimensions - (Vector2(
                boxLayout->origin.x * dimensions.x,
                boxLayout->origin.y * dimensions.y
            ) * 2.0f);
        }

        // Only bother updating layout if bounds actually change.
        if (layout.GetBounds() != bounds)
        {
            layout.SetBounds(bounds);
        }

        if (boxed)
        {
            Rect boxDest = Rect(
                GetTransform()->GetWorldPosition().x - boxPaddingWidth,
                GetTransform()->GetWorldPosition().y - boxPaddingHeight,
                bounds.x + boxPaddingWidth,
                bounds.y + boxPaddingHeight
            );
            boxDest.DrawFilled(renderer, backgroundColor);
        }

        if (font != nullptr)
        {
            /// TODO: figure out why we have to update every frame! Maybe the font atlas glyph cache breaks?
            layout.SetText(renderer, *font, text, applyMarkup);
            layout.Update(*font);

            layout.Render(renderer, *font, GetTransform()->GetWorldPosition());
        }
    }

}
