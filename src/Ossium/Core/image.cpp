extern "C"
{
    #include <SDL2/SDL_image.h>
}

#include "image.h"

namespace Ossium
{

    REGISTER_RESOURCE(Image);

    /// No values or references are copied by default. You have to call Clone() to make a deep copy.
    Image::Image(const Image& source)
    {
    }

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

    bool Image::CreateFromText(Renderer& renderer, Font& font, string text, int pointSize, SDL_Color color, int hinting, int kerning, int outline, int style, int renderMode, SDL_Color bgColor)
    {
        if (pointSize <= 0)
        {
            pointSize = 1;
        }
        TTF_Font* actualFont = font.GetFont(pointSize);
        if (actualFont == NULL)
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
        TTF_SetFontHinting(actualFont, hinting);
        TTF_SetFontKerning(actualFont, (int)kerning);
        TTF_SetFontOutline(actualFont, outline);
        TTF_SetFontStyle(actualFont, style);
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
                    tempSurface = TTF_RenderText_Blended(actualFont, text.c_str(), bgColor);
                    break;
                }
                default:
                {
                    tempSurface = TTF_RenderText_Solid(actualFont, text.c_str(), bgColor);
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
                    Logger::EngineLog().Error("Failed to create texture from surface! SDL_Error: {0}", SDL_GetError());
                }
            }
            else
            {
                Logger::EngineLog().Error("Failed to create texture from text! TTF_Error: {0}", TTF_GetError());
            }
        }
        /// Now do the actual text texture
        TTF_SetFontOutline(actualFont, 0);
        switch (renderMode)
        {
            case RENDERTEXT_SOLID:
            {
                tempSurface = TTF_RenderText_Solid(actualFont, text.c_str(), color);
                break;
            }
            case RENDERTEXT_SHADED:
            {
                tempSurface = TTF_RenderText_Shaded(actualFont, text.c_str(), color, bgColor);
                break;
            }
            case RENDERTEXT_BLEND:
            {
                tempSurface = TTF_RenderText_Blended(actualFont, text.c_str(), color);
                break;
            }
            default:
            {
                tempSurface = TTF_RenderText_Solid(actualFont, text.c_str(), color);
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
            Logger::EngineLog().Error("Failed to create texture from text! TTF_Error: {0}", TTF_GetError());
        }

        return texture != NULL;
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

    bool Image::LockPixels()
    {
        #ifdef OSSIUM_DEBUG
        SDL_assert(texture != NULL);
        #endif
        if (pixels != NULL)
        {
            return false;
        }
        else
        {
            if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0)
            {
                Logger::EngineLog().Error("Image texture lock failure! {0}", SDL_GetError());
                return false;
            }
        }
        return true;
    }

    bool Image::UnlockPixels()
    {
        #ifdef OSSIUM_DEBUG
        SDL_assert(texture != NULL);
        #endif
        if (pixels == NULL)
        {
            return false;
        }
        else
        {
            SDL_UnlockTexture(texture);
            pixels = NULL;
            pitch = 0;
        }
        return true;
    }

}
