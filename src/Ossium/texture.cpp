#include <memory>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "texture.h"
#include "primitives.h"
#include "colours.h"

using namespace std;

namespace Ossium
{

    inline namespace graphics
    {

        ///       ///
        /// IMAGE ///
        ///       ///

        Image::Image()
        {
            tempSurface = NULL;
            texture = NULL;
            outlineTexture = NULL;
            width = 0;
            height = 0;
            pixels = NULL;
            pitch = 0;
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

        bool Image::Load(string guid_path, int* loadArgs)
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
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not load image '%s'! IMG_Error: %s", guid_path.c_str(), IMG_GetError());
            }
            #else
            tempSurface = SDL_LoadBMP(guid_path.c_str());
            if (tempSurface == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not load image '%s'! SDL_Error: %s", guid_path.c_str(), SDL_GetError());
            }
            #endif // SDL_IMAGE_H_
            return tempSurface != NULL;
        }

        bool Image::CreateFromText(Renderer& renderer, Font& font, string text, int pointSize, SDL_Color colour, int hinting, int kerning, int outline, int style, int renderMode, SDL_Color bgColour)
        {
            if (pointSize <= 0)
            {
                pointSize = 1;
            }
            TTF_Font* actualFont = font.getFont(pointSize);
            if (actualFont == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create text. Failure obtaining the font!");
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
                        tempSurface = TTF_RenderText_Blended(actualFont, text.c_str(), bgColour);
                    }
                    default:
                    {
                        tempSurface = TTF_RenderText_Solid(actualFont, text.c_str(), bgColour);
                    }
                }
                if (tempSurface != NULL)
                {
                    width = tempSurface->w;
                    height = tempSurface->h;
                    outlineTexture = SDL_CreateTextureFromSurface(renderer.GetRendererSDL(), tempSurface);
                    SDL_FreeSurface(tempSurface);
                    tempSurface = NULL;
                    if (outlineTexture == NULL)
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface! SDL_Error: %s", SDL_GetError());
                    }
                }
                else
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from text! TTF_Error: %s", TTF_GetError());
                }
            }
            /// Now do the actual text texture
            TTF_SetFontOutline(actualFont, 0);
            switch (renderMode)
            {
                case RENDERTEXT_SOLID:
                {
                    tempSurface = TTF_RenderText_Solid(actualFont, text.c_str(), colour);
                }
                case RENDERTEXT_SHADED:
                {
                    tempSurface = TTF_RenderText_Shaded(actualFont, text.c_str(), colour, bgColour);
                }
                case RENDERTEXT_BLEND:
                {
                    tempSurface = TTF_RenderText_Blended(actualFont, text.c_str(), colour);
                }
                default:
                {
                    tempSurface = TTF_RenderText_Solid(actualFont, text.c_str(), colour);
                }
            }
            if (tempSurface != NULL)
            {
                if (width == 0 || height == 0)
                {
                    width = tempSurface->w;
                    height = tempSurface->h;
                }
                texture = SDL_CreateTextureFromSurface(renderer.GetRendererSDL(), tempSurface);
                SDL_FreeSurface(tempSurface);
                tempSurface = NULL;
                if (texture == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface! SDL_Error: %s", SDL_GetError());
                }
            }
            else
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from text! TTF_Error: %s", TTF_GetError());
            }

            return texture != NULL;
        }

        bool Image::Init(Renderer& renderer, Uint32 windowPixelFormat, bool cache)
        {
            Free();
            format = windowPixelFormat;
            if (tempSurface == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "NULL surface, cannot initialise Image!");
            }
            else if (windowPixelFormat != SDL_PIXELFORMAT_UNKNOWN)
            {
                SDL_Surface* formattedSurface = NULL;
                formattedSurface = SDL_ConvertSurfaceFormat(tempSurface, windowPixelFormat, 0);
                if (formattedSurface == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to format surface! SDL_Error: %s", SDL_GetError());
                }
                else
                {
                    texture = SDL_CreateTexture(renderer.GetRendererSDL(), windowPixelFormat, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
                    if (texture == NULL)
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create Image from surface! SDL_Error: %s", SDL_GetError());
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
                texture = SDL_CreateTextureFromSurface(renderer.GetRendererSDL(), tempSurface);
                if (texture == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create Image from surface! SDL_Error: %s", SDL_GetError());
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

        bool Image::LoadAndInit(string guid_path, Renderer& renderer, Uint32 windowPixelFormat, bool cache)
        {
            return Load(guid_path) && Init(renderer, windowPixelFormat, cache);
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

        bool Image::LockPixels()
        {
            #ifdef DEBUG
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
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Image texture lock failure! %s", SDL_GetError());
                    return false;
                }
            }
            return true;
        }

        bool Image::UnlockPixels()
        {
            #ifdef DEBUG
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

        ///         ///
        /// TEXTURE ///
        ///         ///

        REGISTER_COMPONENT(Texture);

        Texture::Texture()
        {
            source = nullptr;
            modulation = {0xFF, 0xFF, 0xFF, 0xFF};
            blending = SDL_BLENDMODE_BLEND;
            clip = {0, 0, 0, 0};
            flip = SDL_FLIP_NONE;
        }

        Texture::Texture(const Texture& src)
        {
            source = src.source;
            modulation = src.modulation;
            blending = src.blending;
            clip = src.clip;
            flip = src.flip;
        }

        Texture::~Texture()
        {
        }

        void Texture::Render(Renderer& renderer)
        {

            SDL_Rect dest = GetSDL();
            if (source == nullptr || source->texture == NULL)
            {
                SDL_SetRenderDrawColor(renderer.GetRendererSDL(), 255, 100, 255, 255);
                SDL_RenderFillRect(renderer.GetRendererSDL(), &dest);
                return;
            }

            if (source->outlineTexture != NULL)
            {
                SDL_SetTextureBlendMode(source->outlineTexture, blending);
                SDL_SetTextureColorMod(source->outlineTexture, modulation.r, modulation.g, modulation.b);
                SDL_SetTextureAlphaMod(source->outlineTexture, modulation.a);
            }

            SDL_SetTextureBlendMode(source->texture, blending);
            SDL_SetTextureColorMod(source->texture, modulation.r, modulation.g, modulation.b);
            SDL_SetTextureAlphaMod(source->texture, modulation.a);

            SDL_Point trueOrigin = {(int)(origin.x * (float)source->width), (int)(origin.y * (float)source->height)};

            /// Rendering time!
            if (clip.w != 0 && clip.h != 0)
            {
                if (source->outlineTexture != NULL)
                {
                    SDL_RenderCopyEx(renderer.GetRendererSDL(), source->outlineTexture, &clip, &dest, Rotation(), &trueOrigin, flip);
                }
                SDL_RenderCopyEx(renderer.GetRendererSDL(), source->texture, &clip, &dest, Rotation(), &trueOrigin, flip);
            }
            else
            {
                if (source->outlineTexture != NULL)
                {
                    SDL_RenderCopyEx(renderer.GetRendererSDL(), source->outlineTexture, NULL, &dest, Rotation(), &trueOrigin, flip);
                }
                SDL_RenderCopyEx(renderer.GetRendererSDL(), source->texture, NULL, &dest, Rotation(), &trueOrigin, flip);
            }
        }

        ///
        /// Setters
        ///

        void Texture::SetSource(Image* src, bool configureDimensions)
        {
            source = src;
            if (configureDimensions && src != nullptr)
            {
                width = src->width;
                height = src->height;
            }
        }
        void Texture::SetBlendMode(SDL_BlendMode blend)
        {
            blending = blend;
        }
        void Texture::SetAlphaMod(Uint8 a)
        {
            modulation.a = a;
        }
        void Texture::SetColourMod(Uint8 r, Uint8 g, Uint8 b)
        {
            modulation.r = r;
            modulation.g = g;
            modulation.b = b;
        }
        void Texture::SetMod(SDL_Color mod)
        {
            modulation = mod;
        }
        void Texture::SetRenderWidth(float percent)
        {
            width = (int)(percent * (float)source->width);
        }
        void Texture::SetRenderHeight(float percent)
        {
            height = (int)(percent * (float)source->height);
        }
        void Texture::SetFlip(SDL_RendererFlip flipMode)
        {
            flip = flipMode;
        }

        ///
        /// Getters
        ///

        Image* Texture::GetSource()
        {
            return source;
        }
        float Texture::GetRenderWidth()
        {
            return source->width == 0 ? 0 : width / source->width;
        }
        float Texture::GetRenderHeight()
        {
            return source->height == 0 ? 0 : height / source->height;
        }
        SDL_RendererFlip Texture::GetFlip()
        {
            return flip;
        }
        SDL_Color Texture::GetMod()
        {
            return modulation;
        }
        int Texture::GetSourceWidth()
        {
            return source->width;
        }
        int Texture::GetSourceHeight()
        {
            return source->height;
        }

    }

}
