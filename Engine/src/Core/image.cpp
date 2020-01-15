/** COPYRIGHT NOTICE
 *
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
extern "C"
{
    #include <SDL2/SDL_image.h>
}

#include "image.h"

namespace Ossium
{

    TextStyle::TextStyle(string font, int fontSize, SDL_Color color, int hint, int kern, int outlineThickness,
        int styling, int renderingMode, SDL_Color backgroundColor)
    {
        fontPath = font;
        ptsize = fontSize;
        fg = color;
        hinting = hint;
        kerning = kern;
        outline = outlineThickness;
        style = styling;
        rendermode = renderingMode;
        bg = backgroundColor;
    }

    REGISTER_RESOURCE(Image);

    Image::~Image()
    {
        Free();
        if (tempSurface != NULL)
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }
    }

    void Image::Free()
    {
        if (pixels != NULL)
        {
            UnlockPixels();
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
        width = 0;
        height = 0;
    }

    bool Image::Load(string guid_path)
    {
        if (tempSurface != NULL)
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }
        #ifdef SDL_IMAGE_H_
        tempSurface = IMG_Load(guid_path.c_str());
        if (tempSurface == NULL)
        {
            Logger::EngineLog().Error("Could not load image '{0}'! IMG_Error: {1}", guid_path, IMG_GetError());
        }
        #else
        tempSurface = SDL_LoadBMP(guid_path.c_str());
        if (tempSurface == NULL)
        {
            Logger::EngineLog().Error("Could not load image '{0}'! SDL_Error: {1}", guid_path, SDL_GetError());
        }
        #endif // SDL_IMAGE_H_
        else
        {
            pathname = guid_path;
        }
        return tempSurface != NULL;
    }

    bool Image::CreateEmpty(Renderer& renderer, int w, int h, Uint32 pixelFormat)
    {
        Free();
        texture = SDL_CreateTexture(renderer.GetRendererSDL(), pixelFormat, SDL_TEXTUREACCESS_STREAMING);
        if (texture == NULL)
        {
            return false;
        }
        return true;
    }

    bool Image::CreateFromText(Renderer& renderer, TTF_Font* font, string text, SDL_Color color, int hinting, int kerning, int outline, int style, int renderMode, SDL_Color bgColor, Uint32 wrapLength)
    {
        if (font == NULL)
        {
            Logger::EngineLog().Error("Failed to create text. Failure obtaining the font!");
            return false;
        }
        Free();
        if (tempSurface != NULL)
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }
        /// Configure font
        TTF_SetFontHinting(font, hinting);
        TTF_SetFontKerning(font, (int)kerning);
        TTF_SetFontOutline(font, outline);
        TTF_SetFontStyle(font, style);
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
                    tempSurface = TTF_RenderUTF8_Blended(font, text.c_str(), bgColor);
                    break;
                }
                case RENDERTEXT_BLEND_WRAPPED:
                {
                    tempSurface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), bgColor, wrapLength);
                    break;
                }
                default:
                {
                    tempSurface = TTF_RenderUTF8_Solid(font, text.c_str(), bgColor);
                    break;
                }
            }
            if (tempSurface != NULL)
            {
                width = tempSurface->w;
                height = tempSurface->h;
                tempSurface = SDL_ConvertSurfaceFormat(tempSurface, SDL_PIXELFORMAT_ARGB8888, 0);
                outlineTexture = SDL_CreateTextureFromSurface(renderer.GetRendererSDL(), tempSurface);
                SDL_FreeSurface(tempSurface);
                tempSurface = NULL;
                if (outlineTexture == NULL)
                {
                    Logger::EngineLog().Error("Failed to create outline texture from surface! SDL_Error: {0}", SDL_GetError());
                }
            }
            else
            {
                TTF_SizeUTF8(font, text.c_str(), &width, &height);
                if (width != 0 && height != 0)
                {
                    Logger::EngineLog().Error("Failed to create outline texture from text! TTF_Error: {0}", TTF_GetError());
                }
                else
                {
                    // empty text
                }

            }
        }
        /// Now do the actual text texture
        TTF_SetFontOutline(font, 0);
        switch (renderMode)
        {
            case RENDERTEXT_SHADED:
            {
                tempSurface = TTF_RenderUTF8_Shaded(font, text.c_str(), color, bgColor);
                break;
            }
            case RENDERTEXT_BLEND:
            {
                tempSurface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
                break;
            }
            case RENDERTEXT_BLEND_WRAPPED:
            {
                tempSurface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, wrapLength);
                break;
            }
            default:
            {
                tempSurface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
                break;
            }
        }
        if (tempSurface != NULL)
        {
            if (width == 0 || height == 0)
            {
                width = tempSurface->w;
                height = tempSurface->h;
            }
            tempSurface = SDL_ConvertSurfaceFormat(tempSurface, SDL_PIXELFORMAT_ARGB8888, 0);
            texture = SDL_CreateTextureFromSurface(renderer.GetRendererSDL(), tempSurface);
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
            if (texture == NULL)
            {
                Logger::EngineLog().Error("Failed to create texture from surface! SDL_Error: {0}", SDL_GetError());
            }
            else
            {
                pathname = "";
            }
        }
        else
        {
            TTF_SizeUTF8(font, text.c_str(), &width, &height);
            if (width != 0 && height != 0)
            {
                Logger::EngineLog().Error("Failed to create texture from text! TTF_Error: {0}", TTF_GetError());
            }
            else
            {
                width = 1;
                height = 1;
                // Attempted to use invisible characters or empty string, just create a single transparent pixel instead.
                #if SDL_BYTEORDER == SDL_BIG_ENDIAN
                tempSurface = SDL_CreateRGBSurface(0, width, height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
                #else
                tempSurface = SDL_CreateRGBSurface(0, width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
                #endif
                if (tempSurface == NULL)
                {
                    Logger::EngineLog().Error("Failed to create surface from empty text! SDL_Error: {0}", SDL_GetError());
                }
                else
                {
                    texture = SDL_CreateTextureFromSurface(renderer.GetRendererSDL(), tempSurface);
                    SDL_FreeSurface(tempSurface);
                    tempSurface = NULL;
                    if (texture == NULL)
                    {
                        Logger::EngineLog().Error("Failed to create texture from surface! SDL_Error: {0}", SDL_GetError());
                    }
                    else
                    {
                        pathname = "";
                    }
                }
            }
        }

        return texture != NULL;
    }

    bool Image::CreateFromText(Renderer& renderer, Font& font, string text, const TextStyle& style, Uint32 wrapLength)
    {
        return CreateFromText(renderer, font, text, style.ptsize, style.fg, style.hinting, style.kerning, style.outline, style.style, style.rendermode, style.bg, wrapLength);
    }

    bool Image::CreateFromText(Renderer& renderer, Font& font, string text, int pointSize, SDL_Color color, int hinting, int kerning, int outline, int style, int renderMode, SDL_Color bgColor, Uint32 wrapLength)
    {
        return CreateFromText(renderer, font.GetFont(pointSize), text, color, hinting, kerning, outline, style, renderMode, bgColor, wrapLength);
    }

    bool Image::Init(Renderer& renderer, Uint32 pixelFormatting, bool cache)
    {
        Free();
        format = pixelFormatting;
        if (tempSurface == NULL)
        {
            Logger::EngineLog().Error("NULL surface, cannot initialise Image!");
        }
        else if (pixelFormatting != SDL_PIXELFORMAT_UNKNOWN)
        {
            SDL_Surface* formattedSurface = NULL;
            formattedSurface = SDL_ConvertSurfaceFormat(tempSurface, pixelFormatting, 0);
            if (formattedSurface == NULL)
            {
                Logger::EngineLog().Error("Failed to format surface! SDL_Error: {0}", SDL_GetError());
            }
            else
            {
                texture = SDL_CreateTexture(renderer.GetRendererSDL(), pixelFormatting, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
                if (texture == NULL)
                {
                    Logger::EngineLog().Error("Failed to create Image from surface! SDL_Error: {0}", SDL_GetError());
                }
                else
                {
                    /// Grab pixel manipulation data
                    SDL_LockTexture(texture, NULL, &pixels, &pitch);
                    memcpy(pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);
                    SDL_UnlockTexture(texture);
                    pixels = NULL;
                    width = formattedSurface->w;
                    height = formattedSurface->h;
                }
                SDL_FreeSurface(formattedSurface);
                formattedSurface = NULL;
            }
        }
        else
        {
            tempSurface = SDL_ConvertSurfaceFormat(tempSurface, SDL_PIXELFORMAT_ARGB8888, 0);
            texture = SDL_CreateTextureFromSurface(renderer.GetRendererSDL(), tempSurface);
            if (texture == NULL)
            {
                Logger::EngineLog().Error("Failed to create Image from surface! SDL_Error: {0}", SDL_GetError());
            }
            else
            {
                width = tempSurface->w;
                height = tempSurface->h;
            }
        }
        if (tempSurface != NULL && !cache)
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }
        return texture != NULL;
    }

    bool Image::LoadAndInit(string guid_path, Renderer& renderer, Uint32 pixelFormatting, bool cache)
    {
        return Load(guid_path) && Init(renderer, pixelFormatting, cache);
    }

    bool Image::Initialised()
    {
        return texture != NULL;
    }

    void Image::Render(
        SDL_Renderer* renderer,
        SDL_Rect dest,
        SDL_Rect* clip,
        SDL_Point* origin,
        double rotation,
        SDL_Color modulation,
        SDL_BlendMode blending,
        SDL_RendererFlip flip)
    {
        if (texture == NULL)
        {
            SDL_SetRenderDrawColor(renderer, 255, 100, 255, 255);
            SDL_RenderFillRect(renderer, &dest);
            return;
        }

        if (outlineTexture != NULL)
        {
            SDL_SetTextureBlendMode(outlineTexture, blending);
            SDL_SetTextureColorMod(outlineTexture, modulation.r, modulation.g, modulation.b);
            SDL_SetTextureAlphaMod(outlineTexture, modulation.a);
        }

        SDL_SetTextureBlendMode(texture, blending);
        SDL_SetTextureColorMod(texture, modulation.r, modulation.g, modulation.b);
        SDL_SetTextureAlphaMod(texture, modulation.a);

        /// Rendering time!
        if (clip && clip->w > 0 && clip->h > 0)
        {
            if (outlineTexture != NULL)
            {
                SDL_RenderCopyEx(renderer, outlineTexture, clip, &dest, rotation, origin, flip);
            }
            SDL_RenderCopyEx(renderer, texture, clip, &dest, rotation, origin, flip);
        }
        else
        {
            if (outlineTexture != NULL)
            {
                SDL_RenderCopyEx(renderer, outlineTexture, NULL, &dest, rotation, origin, flip);
            }
            SDL_RenderCopyEx(renderer, texture, NULL, &dest, rotation, origin, flip);
        }
    }

    int Image::GetWidth()
    {
        return width;
    }
    int Image::GetHeight()
    {
        return height;
    }

    string Image::GetPathName()
    {
        return pathname;
    }

    SDL_Texture* Image::GetTexture()
    {
        return texture;
    }

    void* Image::GetPixels()
    {
        return pixels;
    }

    int Image::GetPitch()
    {
        return pitch;
    }

    Uint32 Image::GetPixelFormat()
    {
        return format;
    }

    bool Image::LockPixels()
    {
        if (texture == NULL)
        {
            return false;
        }
        else if (pixels == NULL && SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0)
        {
            Logger::EngineLog().Error("Image texture lock failure! {0}", SDL_GetError());
            return false;
        }
        return true;
    }

    bool Image::UnlockPixels()
    {
        if (texture == NULL)
        {
            return false;
        }
        else if (pixels != NULL)
        {
            SDL_UnlockTexture(texture);
            pixels = NULL;
            pitch = 0;
        }
        return true;
    }

}
