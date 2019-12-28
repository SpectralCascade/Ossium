/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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

#include "text.h"
#include "../Core/renderer.h"
#include "../Core/ecs.h"

namespace Ossium
{

    REGISTER_COMPONENT(Text);

    void Text::OnClone()
    {
        /// Set the source image to the newly instantiated, empty image
        source = &image;
        /// We must create the text texture before we can render it
        update = true;
    }

    bool Text::TextToTexture(Renderer& renderer, Font* fontToUse, int pointSize)
    {
        if (update)
        {
            update = false;
        }
        /// If for whatever reason fontToUse is NULL, attempt to use last known font
        if (fontToUse == nullptr)
        {
            fontToUse = font;
            if (fontToUse == nullptr)
            {
                Logger::EngineLog().Error("Cannot render text with NULL font resource!");
                return false;
            }
        }
        else
        {
            font = fontToUse;
        }
        if (pointSize <= 0)
        {
            pointSize = cachedPointSize;
        }
        else
        {
            cachedPointSize = pointSize;
        }
        bool success = image.CreateFromText(renderer, *fontToUse, textData, pointSize, color, hinting, kerning, outline, style, renderMode, bgColor);
        if (success)
        {
            SetSource(&image);
        }
        return success;
    }

    void Text::Render(Renderer& renderer)
    {
        if (update && font != nullptr)
        {
            TextToTexture(renderer, font);
        }
        if (box)
        {
            SDL_Rect boxDest = GetSDL(GetTransform()->GetWorldPosition());
            boxDest.x -= boxPadWidth;
            boxDest.y -= boxPadHeight;
            boxDest.w = GetWidth();
            boxDest.h = GetHeight();
            SDL_SetRenderDrawColor(renderer.GetRendererSDL(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(renderer.GetRendererSDL(), &boxDest);
        }
        Texture::Render(renderer);
    }

    void Text::SetStyling(int textStyle, int textOutline, int textHinting, SDL_Color textColor)
    {
        style = textStyle;
        outline = textOutline;
        hinting = textHinting;
        color = textColor;
        update = true;
    }

    ///
    /// START OF GET/SET METHODS
    ///

    /// GET METHODS

    string Text::GetText()
    {
        return textData;
    }
    SDL_Color Text::GetColor()
    {
        return color;
    }
    SDL_Color Text::GetBackgroundColor()
    {
        return bgColor;
    }
    int Text::GetWidth()
    {
        return width + ((int)box * boxPadWidth * 2);
    }
    int Text::GetHeight()
    {
        return height + ((int)box * boxPadHeight * 2);
    }
    int Text::GetOutline()
    {
        return outline;
    }
    int Text::GetStyle()
    {
        return style;
    }
    int Text::GetHinting()
    {
        return hinting;
    }
    int Text::GetRenderMode()
    {
        return renderMode;
    }
    int Text::GetBoxPaddingWidth()
    {
        return boxPadWidth;
    }
    int Text::GetBoxPaddingHeight()
    {
        return boxPadHeight;
    }
    bool Text::IsKerning()
    {
        return kerning;
    }
    bool Text::IsBoxed()
    {
        return box;
    }

    /// SET METHODS

    void Text::SetText(string text)
    {
        if (text != textData)
        {
            textData = text;
            update = true;
        }
    }
    void Text::SetColor(SDL_Color textColor)
    {
        color = textColor;
        update = true;
    }
    void Text::SetBackgroundColor(SDL_Color textBackgroundColor)
    {
        bgColor = textBackgroundColor;
        if (renderMode == RENDERTEXT_SHADED || box || outline > 0)
        {
            update = true;
        }
    }
    void Text::SetOutline(int textOutine)
    {
        outline = textOutine;
        update = true;
    }
    void Text::SetStyle(int textStyle)
    {
        style = textStyle;
        update = true;
    }
    void Text::SetHinting(int textHinting)
    {
        hinting = textHinting;
        update = true;
    }
    void Text::SetRenderMode(int textRenderMode)
    {
        renderMode = textRenderMode;
        update = true;
    }
    void Text::SetBoxPaddingWidth(int padWidth)
    {
        boxPadWidth = padWidth;
    }
    void Text::SetBoxPaddingHeight(int padHeight)
    {
        boxPadHeight = padHeight;
    }
    void Text::SetKerning(bool enabled)
    {
        kerning = enabled;
        update = true;
    }
    void Text::SetBox(bool enabled)
    {
        box = enabled;
    }

    ///
    /// END OF GET/SET METHODS
    ///

}
