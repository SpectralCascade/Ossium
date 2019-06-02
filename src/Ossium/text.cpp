#include <string>
#include <algorithm>
#include <SDL.h>
#include <SDL_ttf.h>

#include "text.h"
#include "renderer.h"
#include "ecs.h"

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
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Cannot render text with NULL font resource!");
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
        return false;
    }

    void Text::Render(Renderer& renderer)
    {
        if (update && font != nullptr)
        {
            TextToTexture(renderer, font);
        }
        if (box)
        {
            SDL_Rect boxDest = GetSDL();
            boxDest.x -= boxPadWidth;
            boxDest.y -= boxPadHeight;
            boxDest.w += 2 * boxPadWidth;
            boxDest.h += 2 * boxPadHeight;
            SDL_SetRenderDrawColor(renderer.GetRendererSDL(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(renderer.GetRendererSDL(), &boxDest);
            position.x += boxPadWidth;
            position.y += boxPadHeight;
        }
        Texture::Render(renderer);
        if (box)
        {
            position.x -= boxPadWidth;
            position.y -= boxPadHeight;
        }
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
