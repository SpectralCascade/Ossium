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

    Text::Text()
    {
        textData = "";
        update = true;
        color = {0xFF, 0xFF, 0xFF, 0xFF};
        bgColor = {0x00, 0x00, 0x00, 0xFF};
        font = NULL;
        style = TTF_STYLE_NORMAL;
        hinting = TTF_HINTING_NORMAL;
        renderMode = RENDERTEXT_SOLID;
        outline = 0;
        boxPadWidth = 4;
        boxPadHeight = 2;
        kerning = true;
        box = false;
        cachedPointSize = 24;
    }

    Text::Text(const Text& copySource)
    {
        bgColor = copySource.bgColor;
        box = copySource.box;
        boxPadHeight = copySource.boxPadWidth;
        boxPadWidth = copySource.boxPadWidth;
        color = copySource.color;
        font = copySource.font;
        font_guid = copySource.font_guid;
        hinting = copySource.hinting;
        kerning = copySource.kerning;
        outline = copySource.outline;
        renderMode = copySource.renderMode;
        style = copySource.style;
        textData = copySource.textData;
        update = true;
        cachedPointSize = copySource.cachedPointSize;
        /// No need to NULL the font, as the font is not managed by this class
    }

    Text::~Text()
    {
        if (font != NULL)
        {
            /// Don't close the font, as other Text objects may be using it still.
            /// Leave that job to the font resource manager
            font = NULL;
        }
    }

    bool Text::textToTexture(Renderer& renderer, Font* fontToUse, int pointSize)
    {
        if (update)
        {
            update = false;
        }
        /// If for whatever reason fontToUse is NULL, attempt to use last known font
        if (fontToUse == NULL)
        {
            fontToUse = font;
            if (fontToUse == NULL)
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
        if (update && font != NULL)
        {
            textToTexture(renderer, font);
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

    void Text::setStyling(int textStyle, int textOutline, int textHinting, SDL_Color textColor)
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

    string Text::getText()
    {
        return textData;
    }
    SDL_Color Text::getColor()
    {
        return color;
    }
    SDL_Color Text::getBackgroundColor()
    {
        return bgColor;
    }
    int Text::getWidth()
    {
        return width + ((int)box * boxPadWidth * 2);
    }
    int Text::getHeight()
    {
        return height + ((int)box * boxPadHeight * 2);
    }
    int Text::getOutline()
    {
        return outline;
    }
    int Text::getStyle()
    {
        return style;
    }
    int Text::getHinting()
    {
        return hinting;
    }
    int Text::getRenderMode()
    {
        return renderMode;
    }
    int Text::getBoxPaddingWidth()
    {
        return boxPadWidth;
    }
    int Text::getBoxPaddingHeight()
    {
        return boxPadHeight;
    }
    bool Text::isKerning()
    {
        return kerning;
    }
    bool Text::isBoxed()
    {
        return box;
    }

    /// SET METHODS

    void Text::setText(string text)
    {
        textData = text;
        update = true;
    }
    void Text::setColor(SDL_Color textColor)
    {
        color = textColor;
        update = true;
    }
    void Text::setBackgroundColor(SDL_Color textBackgroundColor)
    {
        bgColor = textBackgroundColor;
        if (renderMode == RENDERTEXT_SHADED || box || outline > 0)
        {
            update = true;
        }
    }
    void Text::setOutline(int textOutine)
    {
        outline = textOutine;
        update = true;
    }
    void Text::setStyle(int textStyle)
    {
        style = textStyle;
        update = true;
    }
    void Text::setHinting(int textHinting)
    {
        hinting = textHinting;
        update = true;
    }
    void Text::setRenderMode(int textRenderMode)
    {
        renderMode = textRenderMode;
        update = true;
    }
    void Text::setBoxPaddingWidth(int padWidth)
    {
        boxPadWidth = padWidth;
    }
    void Text::setBoxPaddingHeight(int padHeight)
    {
        boxPadHeight = padHeight;
    }
    void Text::setKerning(bool enabled)
    {
        kerning = enabled;
        update = true;
    }
    void Text::setBox(bool enabled)
    {
        box = enabled;
    }

    ///
    /// END OF GET/SET METHODS
    ///

}
