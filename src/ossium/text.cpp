#include <string>
#include <algorithm>
#include <SDL.h>
#include <SDL_ttf.h>

#include "text.h"
#include "renderer.h"
#include "ecs.h"

namespace ossium
{
    REGISTER_COMPONENT(Text);

    Text::Text()
    {
        textData = "";
        update = false;
        color = {0xFF, 0xFF, 0xFF, 0xFF};
        bgColor = {0x00, 0x00, 0x00, 0xFF};
        texture = NULL;
        outlineTexture = NULL;
        font = NULL;
        style = TTF_STYLE_NORMAL;
        hinting = TTF_HINTING_NORMAL;
        renderMode = RENDERTEXT_SOLID;
        width = 0;
        height = 0;
        outlineWidth = 0;
        outlineHeight = 0;
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
        height = copySource.height;
        width = copySource.width;
        hinting = copySource.hinting;
        kerning = copySource.kerning;
        outline = copySource.outline;
        outlineHeight = copySource.outlineHeight;
        outlineWidth = copySource.outlineWidth;
        renderMode = copySource.renderMode;
        style = copySource.style;
        textData = copySource.textData;
        update = copySource.update;
        cachedPointSize = copySource.cachedPointSize;
        /// Next time a render or textToTexture() method is called, these texture(s) are created
        texture = NULL;
        outlineTexture = NULL;
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
        if (texture != NULL)
        {
            SDL_DestroyTexture(texture);
            texture = NULL;
        }
        if (outlineTexture != NULL)
        {
            SDL_DestroyTexture(outlineTexture);
            outlineTexture = NULL;
        }
    }

    bool Text::textToTexture(Renderer* renderer, Font* fontToUse, int pointSize)
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
        if (pointSize <= 0)
        {
            pointSize = cachedPointSize;
        }
        else
        {
            cachedPointSize = pointSize;
        }
        TTF_Font* actualFont = fontToUse->getFont(pointSize);
        if (actualFont == NULL)
        {
            return false;
        }
        font = fontToUse;

        /// Free texture
        if (texture != NULL)
        {
            SDL_DestroyTexture(texture);
            texture = NULL;
        }
        /// Configure font
        TTF_SetFontHinting(actualFont, hinting);
        TTF_SetFontKerning(actualFont, (int)kerning);
        TTF_SetFontOutline(actualFont, outline);
        TTF_SetFontStyle(actualFont, style);
        /// Now render the text to a surface - do the outline first if one exists and we're not doing a shaded render
        SDL_Surface* textSurface = NULL;
        if (outline > 0 && renderMode != RENDERTEXT_SHADED)
        {
            if (outlineTexture != NULL)
            {
                SDL_DestroyTexture(outlineTexture);
                outlineTexture = NULL;
            }
            switch (renderMode)
            {
                case RENDERTEXT_BLEND:
                {
                    textSurface = TTF_RenderText_Blended(actualFont, textData.c_str(), bgColor);
                }
                default:
                {
                    textSurface = TTF_RenderText_Solid(actualFont, textData.c_str(), bgColor);
                }
            }
            if (textSurface != NULL)
            {
                outlineWidth = textSurface->w;
                outlineHeight = textSurface->h;
                outlineTexture = SDL_CreateTextureFromSurface(renderer->getRenderer(), textSurface);
                SDL_FreeSurface(textSurface);
                textSurface = NULL;
                if (outlineTexture == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface! SDL_Error: %s", SDL_GetError());
                }
            }
            else
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, TTF_GetError());
            }
        }
        /// Now do the actual text texture
        TTF_SetFontOutline(actualFont, 0);
        switch (renderMode)
        {
            case RENDERTEXT_SOLID:
            {
                textSurface = TTF_RenderText_Solid(actualFont, textData.c_str(), color);
            }
            case RENDERTEXT_SHADED:
            {
                textSurface = TTF_RenderText_Shaded(actualFont, textData.c_str(), color, bgColor);
            }
            case RENDERTEXT_BLEND:
            {
                textSurface = TTF_RenderText_Blended(actualFont, textData.c_str(), color);
            }
            default:
            {
                textSurface = TTF_RenderText_Solid(actualFont, textData.c_str(), color);
            }
        }
        if (textSurface != NULL)
        {
            width = textSurface->w;
            height = textSurface->h;
            texture = SDL_CreateTextureFromSurface(renderer->getRenderer(), textSurface);
            SDL_FreeSurface(textSurface);
            textSurface = NULL;
            if (texture == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface! SDL_Error: %s", SDL_GetError());
            }
        }
        else
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, TTF_GetError());
        }

        return texture != NULL;
    }

    void Text::render(Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, int layer, float angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        if (!texture)
        {
            textToTexture(renderer, font);
        }
        renderer->enqueueEx(this, dest, clip == NULL ? (SDL_Rect){0, 0, width, height} : *clip, layer, angle, origin == NULL ? (SDL_Point){width / 2, height / 2} : *origin, flip);
    }

    void Text::render(Renderer* renderer, int x, int y, SDL_Rect* clip, int layer, float angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        render(renderer, clip == NULL ? (SDL_Rect){x, y, width, height} : (SDL_Rect){x, y, clip->w, clip->h}, clip, layer, angle, origin, flip);
    }

    void Text::renderSimple(Renderer* renderer, int x, int y, SDL_Rect* clip, int layer)
    {
        renderSimple(renderer, clip == NULL ? (SDL_Rect){x, y, width, height} : (SDL_Rect){x, y, clip->w, clip->h}, clip, layer);
    }

    void Text::renderSimple(Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, int layer)
    {
        if (!texture)
        {
            textToTexture(renderer, font);
        }
        renderer->enqueue(this, dest, clip == NULL ? (SDL_Rect){0, 0, width, height} : *clip, layer);
    }

    void Text::setStyling(int textStyle, int textOutline, int textHinting, SDL_Color textColor)
    {
        style = textStyle;
        outline = textOutline;
        hinting = textHinting;
        color = textColor;
        update = true;
        /// Destroy outline texture
        if (outlineTexture != NULL)
        {
            SDL_DestroyTexture(outlineTexture);
            outlineTexture = NULL;
        }
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
        if (outlineTexture != NULL)
        {
            SDL_DestroyTexture(outlineTexture);
            outlineTexture = NULL;
        }
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

    ///
    /// PRIVATE RENDER METHODS
    /// Accessible by friend class Renderer
    ///

    void Text::renderText(Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        if (update && font != NULL)
        {
            textToTexture(renderer, font);
        }
        SDL_assert(texture != NULL);
        SDL_assert(renderer != NULL);
        if (box)
        {
            dest.x += boxPadWidth;
            dest.y += boxPadHeight;
            SDL_Rect boxDest = {dest.x - boxPadWidth, dest.y - boxPadHeight, dest.w + (2 * boxPadWidth), dest.h + (2 * boxPadHeight)};
            SDL_SetRenderDrawColor(renderer->getRenderer(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(renderer->getRenderer(), &boxDest);
        }
        if (clip != NULL)
        {
            if (outline > 0 && !box)
            {
                SDL_assert(outlineTexture != NULL);
                SDL_RenderCopyEx(renderer->getRenderer(), outlineTexture, clip, &dest, angle, origin, flip);
            }
            SDL_RenderCopyEx(renderer->getRenderer(), texture, clip, &dest, angle, origin, flip);
        }
        else
        {
            SDL_Rect src;
            if (outline > 0 && !box)
            {
                SDL_assert(outlineTexture != NULL);
                src = {0, 0, outlineWidth, outlineHeight};
                SDL_RenderCopyEx(renderer->getRenderer(), outlineTexture, clip, &dest, angle, origin, flip);
            }
            src = {0, 0, width, height};
            SDL_RenderCopyEx(renderer->getRenderer(), texture, clip, &dest, angle, origin, flip);
        }
    }

    void Text::renderText(Renderer* renderer, int x, int y, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        SDL_Rect dest = {x, y, width, height};
        if (clip != NULL)
        {
            dest.w = clip->w;
            dest.h = clip->h;
        }
        renderText(renderer, dest, clip, angle, origin, flip);
    }

    void Text::renderTextSimple(Renderer* renderer, int x, int y, SDL_Rect* clip)
    {
        renderTextSimple(renderer, clip == NULL ? (SDL_Rect){x, y, width, height} : (SDL_Rect){x, y, clip->w, clip->h}, clip);
    }

    void Text::renderTextSimple(Renderer* renderer, SDL_Rect dest, SDL_Rect* clip)
    {
        if (update && font != NULL)
        {
            textToTexture(renderer, font);
        }
        SDL_assert(texture != NULL);
        SDL_assert(renderer != NULL);
        if (box)
        {
            dest.x += boxPadWidth;
            dest.y += boxPadHeight;
            SDL_Rect boxDest = {dest.x - boxPadWidth, dest.y - boxPadHeight, dest.w + (2 * boxPadWidth), dest.h + (2 * boxPadHeight)};
            SDL_SetRenderDrawColor(renderer->getRenderer(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(renderer->getRenderer(), &boxDest);
        }
        if (clip != NULL)
        {
            if (outline > 0 && !box)
            {
                SDL_assert(outlineTexture != NULL);
                SDL_RenderCopy(renderer->getRenderer(), outlineTexture, clip, &dest);
            }
            SDL_RenderCopy(renderer->getRenderer(), texture, clip, &dest);
        }
        else
        {
            SDL_Rect src;
            if (outline > 0 && !box)
            {
                SDL_assert(outlineTexture != NULL);
                src = {0, 0, outlineWidth, outlineHeight};
                SDL_RenderCopy(renderer->getRenderer(), outlineTexture, &src, &dest);
            }
            src = {0, 0, width, height};
            SDL_RenderCopy(renderer->getRenderer(), texture, &src, &dest);
        }
    }

}
