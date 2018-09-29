#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "oss_text.h"

OSS_Text::OSS_Text()
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
    renderMode = OSS_RENDERTEXT_SOLID;
    width = 0;
    height = 0;
    outlineWidth = 0;
    outlineHeight = 0;
    outline = 0;
    boxPadWidth = 4;
    boxPadHeight = 2;
    kerning = true;
    box = false;
}

OSS_Text::~OSS_Text()
{
    if (font != NULL)
    {
        /// Don't close the font, as other OSS_Text objects may be using it still.
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

/// PLACEHOLDERS!!! TODO: Implement these methods
bool OSS_Text::load(string guid_path)
{
    return true;
}

bool OSS_Text::init(string guid_path)
{
    return true;
}

bool OSS_Text::textToTexture(SDL_Renderer* renderer, OSS_Font* fontToUse)
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
    TTF_Font* actualFont = fontToUse->getFont();
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
    if (outline > 0 && renderMode != OSS_RENDERTEXT_SHADED)
    {
        if (outlineTexture != NULL)
        {
            SDL_DestroyTexture(outlineTexture);
            outlineTexture = NULL;
        }
        switch (renderMode)
        {
            case OSS_RENDERTEXT_BLEND:
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
            outlineTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
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
        case OSS_RENDERTEXT_SOLID:
        {
            textSurface = TTF_RenderText_Solid(actualFont, textData.c_str(), color);
        }
        case OSS_RENDERTEXT_SHADED:
        {
            textSurface = TTF_RenderText_Shaded(actualFont, textData.c_str(), color, bgColor);
        }
        case OSS_RENDERTEXT_BLEND:
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
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
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

void OSS_Text::render(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
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
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &dest);
    }
    if (clip != NULL)
    {
        if (outline > 0 && !box)
        {
            SDL_assert(outlineTexture != NULL);
            SDL_RenderCopyEx(renderer, outlineTexture, clip, &dest, angle, origin, flip);
        }
        SDL_RenderCopyEx(renderer, texture, clip, &dest, angle, origin, flip);
    }
    else
    {
        SDL_Rect src;
        if (outline > 0 && !box)
        {
            SDL_assert(outlineTexture != NULL);
            src = {0, 0, outlineWidth, outlineHeight};
            SDL_RenderCopyEx(renderer, outlineTexture, &src, &dest, angle, origin, flip);
        }
        src = {0, 0, width, height};
        SDL_RenderCopyEx(renderer, texture, &src, &dest, angle, origin, flip);
    }
}

void OSS_Text::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
{
    SDL_Rect dest = {x, y, width, height};
    if (clip != NULL)
    {
        dest.w = clip->w;
        dest.h = clip->h;
    }
    render(renderer, dest, clip, angle, origin, flip);
}

void OSS_Text::renderSimple(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip)
{
    if (update && font != NULL)
    {
        textToTexture(renderer, font);
    }
    SDL_assert(texture != NULL);
    SDL_assert(renderer != NULL);
    SDL_Rect dest = {x, y, width, height};
    if (clip != NULL)
    {
        dest.w = clip->w;
        dest.h = clip->h;
    }
    if (box)
    {
        dest.x += boxPadWidth;
        dest.y += boxPadHeight;
        SDL_Rect boxDest = {dest.x - boxPadWidth, dest.y - boxPadHeight, dest.w + (2 * boxPadWidth), dest.h + (2 * boxPadHeight)};
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &boxDest);
    }
    if (clip != NULL)
    {
        if (outline > 0 && !box)
        {
            SDL_assert(outlineTexture != NULL);
            SDL_RenderCopy(renderer, outlineTexture, clip, &dest);
        }
        SDL_RenderCopy(renderer, texture, clip, &dest);
    }
    else
    {
        SDL_Rect src;
        if (outline > 0 && !box)
        {
            SDL_assert(outlineTexture != NULL);
            src = {0, 0, outlineWidth, outlineHeight};
            SDL_RenderCopy(renderer, outlineTexture, &src, &dest);
        }
        src = {0, 0, width, height};
        SDL_RenderCopy(renderer, texture, &src, &dest);
    }
}

void OSS_Text::setStyling(int textStyle, int textOutline, int textHinting, SDL_Color textColor)
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

string OSS_Text::getText()
{
    return textData;
}
SDL_Color OSS_Text::getColor()
{
    return color;
}
SDL_Color OSS_Text::getBackgroundColor()
{
    return bgColor;
}
int OSS_Text::getWidth()
{
    return width + ((int)box * boxPadWidth * 2);
}
int OSS_Text::getHeight()
{
    return height + ((int)box * boxPadHeight * 2);
}
int OSS_Text::getOutline()
{
    return outline;
}
int OSS_Text::getStyle()
{
    return style;
}
int OSS_Text::getHinting()
{
    return hinting;
}
int OSS_Text::getRenderMode()
{
    return renderMode;
}
int OSS_Text::getBoxPaddingWidth()
{
    return boxPadWidth;
}
int OSS_Text::getBoxPaddingHeight()
{
    return boxPadHeight;
}
bool OSS_Text::isKerning()
{
    return kerning;
}
bool OSS_Text::isBoxed()
{
    return box;
}

/// SET METHODS

void OSS_Text::setText(string text)
{
    textData = text;
    update = true;
}
void OSS_Text::setColor(SDL_Color textColor)
{
    color = textColor;
    update = true;
}
void OSS_Text::setBackgroundColor(SDL_Color textBackgroundColor)
{
    bgColor = textBackgroundColor;
    if (renderMode == OSS_RENDERTEXT_SHADED || outline > 0)
    {
        update = true;
    }
}
void OSS_Text::setOutline(int textOutine)
{
    outline = textOutine;
    update = true;
    if (outlineTexture != NULL)
    {
        SDL_DestroyTexture(outlineTexture);
        outlineTexture = NULL;
    }
}
void OSS_Text::setStyle(int textStyle)
{
    style = textStyle;
    update = true;
}
void OSS_Text::setHinting(int textHinting)
{
    hinting = textHinting;
    update = true;
}
void OSS_Text::setRenderMode(int textRenderMode)
{
    renderMode = textRenderMode;
    update = true;
}
void OSS_Text::setBoxPaddingWidth(int padWidth)
{
    boxPadWidth = padWidth;
}
void OSS_Text::setBoxPaddingHeight(int padHeight)
{
    boxPadHeight = padHeight;
}
void OSS_Text::setKerning(bool enabled)
{
    kerning = enabled;
    update = true;
}
void OSS_Text::setBox(bool enabled)
{
    box = enabled;
}

///
/// END OF GET/SET METHODS
///
