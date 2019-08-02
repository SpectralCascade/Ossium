#include <memory>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "texture.h"
#include "colors.h"

using namespace std;

namespace Ossium
{

    inline namespace Graphics
    {

        ///       ///
        /// IMAGE ///
        ///       ///

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
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not load image '%s'! IMG_Error: %s", guid_path.c_str(), IMG_GetError());
            }
            #else
            tempSurface = SDL_LoadBMP(guid_path.c_str());
            if (tempSurface == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not load image '%s'! SDL_Error: %s", guid_path.c_str(), SDL_GetError());
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
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface! SDL_Error: %s", SDL_GetError());
                }
                else
                {
                    pathname = "";
                }
            }
            else
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from text! TTF_Error: %s", TTF_GetError());
            }

            return texture != NULL;
        }

        bool Image::Init(Renderer& renderer, Uint32 pixelFormatting, bool cache)
        {
            Free();
            format = pixelFormatting;
            if (tempSurface == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "NULL surface, cannot initialise Image!");
            }
            else if (pixelFormatting != SDL_PIXELFORMAT_UNKNOWN)
            {
                SDL_Surface* formattedSurface = NULL;
                formattedSurface = SDL_ConvertSurfaceFormat(tempSurface, pixelFormatting, 0);
                if (formattedSurface == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to format surface! SDL_Error: %s", SDL_GetError());
                }
                else
                {
                    texture = SDL_CreateTexture(renderer.GetRendererSDL(), pixelFormatting, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
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
                tempSurface = SDL_ConvertSurfaceFormat(tempSurface, SDL_PIXELFORMAT_ARGB8888, 0);
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

            SDL_Point trueOrigin = {(int)(origin.x * width), (int)(origin.y * height)};

            /// Rendering time!
            if (clip.w > 0 && clip.h > 0)
            {
                if (source->outlineTexture != NULL)
                {
                    SDL_RenderCopyEx(renderer.GetRendererSDL(), source->outlineTexture, &clip, &dest, angle, &trueOrigin, flip);
                }
                SDL_RenderCopyEx(renderer.GetRendererSDL(), source->texture, &clip, &dest, angle, &trueOrigin, flip);
            }
            else
            {
                if (source->outlineTexture != NULL)
                {
                    SDL_RenderCopyEx(renderer.GetRendererSDL(), source->outlineTexture, NULL, &dest, angle, &trueOrigin, flip);
                }
                SDL_RenderCopyEx(renderer.GetRendererSDL(), source->texture, NULL, &dest, angle, &trueOrigin, flip);
            }
        }

        void Texture::Update()
        {
        }

        ///
        /// Setters
        ///

        void Texture::SetSource(Image* src, bool configureDimensions)
        {
            source = src;
            if (configureDimensions)
            {
                clip.x = 0;
                clip.y = 0;
                if (src != nullptr)
                {
                    width = src->width;
                    height = src->height;
                    clip.w = src->width;
                    clip.h = src->height;
                }
                else
                {
                    /// Make invisible if the source is null
                    width = 0;
                    height = 0;
                    clip.w = 0;
                    clip.h = 0;
                }
            }
        }
        void Texture::SetBlendMode(SDL_BlendMode blend, bool immediate)
        {
            if (immediate)
            {
                SDL_SetTextureBlendMode(source->texture, blend);
            }
            blending = blend;
        }
        void Texture::SetAlphaMod(Uint8 a, bool immediate)
        {
            if (immediate)
            {
                SDL_SetTextureAlphaMod(source->texture, a);
            }
            modulation.a = a;
        }
        void Texture::SetColorMod(Uint8 r, Uint8 g, Uint8 b, bool immediate)
        {
            if (immediate)
            {
                SDL_SetTextureColorMod(source->texture, r, g, b);
            }
            modulation.r = r;
            modulation.g = g;
            modulation.b = b;
        }
        void Texture::SetMod(SDL_Color mod, bool immediate)
        {
            if (immediate)
            {
                SDL_SetTextureColorMod(source->texture, mod.r, mod.g, mod.b);
                SDL_SetTextureAlphaMod(source->texture, mod.a);
            }
            modulation = mod;
        }
        void Texture::SetRenderWidth(float percent)
        {
            width = (int)(percent * (float)clip.w);
        }
        void Texture::SetRenderHeight(float percent)
        {
            height = (int)(percent * (float)clip.h);
        }
        void Texture::SetFlip(SDL_RendererFlip flipMode)
        {
            flip = flipMode;
        }
        void Texture::SetClip(int x, int y, int w, int h, bool autoScale)
        {
            if (autoScale)
            {
                /// Cache percentage width and height
                float wpercent = clip.w == 0 ? 0 : width / (float)clip.w;
                float hpercent = clip.h == 0 ? 0 : height / (float)clip.h;
                clip = {x, y, w, h};
                /// Recalculate destination dimensions with new clip rect
                SetRenderWidth(wpercent);
                SetRenderHeight(hpercent);
            }
            else
            {
                clip = {x, y, w, h};
            }
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
            return clip.w == 0 ? 0 : width / (float)clip.w;
        }
        float Texture::GetRenderHeight()
        {
            return clip.h == 0 ? 0 : height / (float)clip.h;
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
        SDL_Rect Texture::GetClip()
        {
            return clip;
        }

        Point Texture::ScreenToLocalPoint(Point source)
        {
            source.x = (source.x - position.x + (width * 0.5f)) / (width / (float)(clip.w == 0 ? 0 : clip.w));
            source.y = (source.y - position.y + (height * 0.5f)) / (height / (float)(clip.h == 0 ? 0 : clip.h));
            return source;
        }

    }

}
