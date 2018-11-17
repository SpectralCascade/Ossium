#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "texture.h"
#include "renderer.h"

using namespace std;

namespace ossium
{

    Texture::Texture()
    {
        tempSurface = NULL;
        texture = NULL;
        width = 0;
        height = 0;
        pixels = NULL;
        pitch = 0;
    }

    Texture::~Texture()
    {
        freeTexture();
        if (tempSurface != NULL)
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }
    }

    void Texture::freeTexture()
    {
        if (texture != NULL)
        {
            SDL_DestroyTexture(texture);
            texture = NULL;
        }
    }

    bool Texture::load(string guid_path, int* loadArgs)
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

    bool Texture::init(Renderer* renderer, Uint32 windowPixelFormat)
    {
        // pixel format stuff not working currently - disabling it
        //windowPixelFormat = SDL_PIXELFORMAT_UNKNOWN;
        #ifdef DEBUG
        SDL_assert(renderer != NULL);
        #endif
        freeTexture();
        if (tempSurface == NULL)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "NULL surface image, cannot initialise texture!");
        }
        else if (windowPixelFormat != SDL_PIXELFORMAT_UNKNOWN)
        {
            SDL_Surface* formattedSurface = NULL;
            formattedSurface = SDL_ConvertSurfaceFormat(tempSurface, windowPixelFormat, NULL);
            if (formattedSurface == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to format surface! SDL_Error: %s", SDL_GetError());
            }
            else
            {
                texture = SDL_CreateTexture(renderer->getRenderer(), windowPixelFormat, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
                if (texture == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface! SDL_Error: %s", SDL_GetError());
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
            texture = SDL_CreateTextureFromSurface(renderer->getRenderer(), tempSurface);
            if (texture == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface! SDL_Error: %s", SDL_GetError());
            }
            else
            {
                width = tempSurface->w;
                height = tempSurface->h;
            }
        }
        if (texture != NULL)
        {
            /// Enable hardware-accelerated alpha blending by default
            setBlendMode(SDL_BLENDMODE_BLEND);
        }
        if (tempSurface != NULL)
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }
        return texture != NULL;
    }

    #ifdef _SDL_TTF_H
    bool Texture::createText(string text, Renderer* renderer, TTF_Font* font, SDL_Color color)
    {
        freeTexture();
        SDL_Texture* newTexture = NULL;
        SDL_Surface* textSurface = NULL;
        textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (textSurface == NULL)
        {
            newTexture = SDL_CreateTextureFromSurface(renderer->getRenderer(), textSurface);
            if (newTexture == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
            }
            else
            {
                width = textSurface->w;
                height = textSurface->h;
            }
            SDL_FreeSurface(textSurface);
        }
        texture = newTexture;
        return texture != NULL;
    }
    #endif // _SDL_TTF_H

    void Texture::render(Renderer* renderer, int x, int y, SDL_Rect* clip, int layer, float angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        render(renderer, clip == NULL ? (SDL_Rect){x, y, width, height} : (SDL_Rect){x, y, clip->w, clip->h}, clip, layer, angle, origin, flip);
    }

    void Texture::render(Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, int layer, float angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        renderer->enqueueEx(this, dest, clip == NULL ? (SDL_Rect){0, 0, width, height} : *clip, layer, angle, origin == NULL ? (SDL_Point){width / 2, height / 2} : *origin, flip);
    }

    void Texture::renderSimple(Renderer* renderer, int x, int y, SDL_Rect* clip, int layer)
    {
        renderSimple(renderer, clip == NULL ? (SDL_Rect){x, y, width, height} : (SDL_Rect){x, y, clip->w, clip->h}, clip, layer);
    }

    void Texture::renderSimple(Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, int layer)
    {
        renderer->enqueue(this, dest, clip == NULL ? (SDL_Rect){0, 0, width, height} : *clip, layer);
    }

    void Texture::setBlendMode(SDL_BlendMode blend)
    {
        #ifdef DEBUG
        SDL_assert(texture != NULL);
        #endif
        SDL_SetTextureBlendMode(texture, blend);
    }

    void Texture::setAlpha(Uint8 a)
    {
        #ifdef DEBUG
        SDL_assert(texture != NULL);
        #endif
        SDL_SetTextureAlphaMod(texture, a);
    }

    void Texture::setColorMod(Uint8 r, Uint8 g, Uint8 b)
    {
        #ifdef DEBUG
        SDL_assert(texture != NULL);
        #endif
        SDL_SetTextureColorMod(texture, r, g, b);
    }

    int Texture::getWidth()
    {
        return width;
    }

    int Texture::getHeight()
    {
        return height;
    }

    bool Texture::lockPixels()
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
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture lock failure, %s", SDL_GetError());
                return false;
            }
        }
        return true;
    }

    bool Texture::unlockPixels()
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

    void* Texture::getPixels()
    {
        return pixels;
    }

    int Texture::getPitch()
    {
        return pitch;
    }

    ///
    /// PRIVATE RENDERING METHODS, utilised by friend class Renderer
    ///

    void Texture::renderTexture(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        #ifdef DEBUG
        SDL_assert(texture != NULL);
        SDL_assert(renderer != NULL);
        #endif
        if (clip != NULL)
        {
            if (dest.w == 0)
            {
                dest.w = clip->w;
            }
            if (dest.h == 0)
            {
                dest.h = clip->h;
            }
            SDL_RenderCopyEx(renderer, texture, clip, &dest, angle, origin, flip);
        }
        else
        {
            SDL_Rect src = {0, 0, width, height};
            SDL_RenderCopyEx(renderer, texture, &src, &dest, angle, origin, flip);
        }
    }

    void Texture::renderTexture(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
    {
        #ifdef DEBUG
        SDL_assert(texture != NULL);
        SDL_assert(renderer != NULL);
        #endif
        SDL_Rect dest = {x, y, width, height};
        if (clip != NULL)
        {
            dest.w = clip->w;
            dest.h = clip->h;
            SDL_RenderCopyEx(renderer, texture, clip, &dest, angle, origin, flip);
        }
        else
        {
            SDL_Rect src = {0, 0, width, height};
            SDL_RenderCopyEx(renderer, texture, &src, &dest, angle, origin, flip);
        }
    }

    void Texture::renderTextureSimple(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip)
    {
        #ifdef DEBUG
        SDL_assert(texture != NULL);
        SDL_assert(renderer != NULL);
        #endif
        SDL_Rect dest = {x, y, width, height};
        if (clip != NULL)
        {
            dest.w = clip->w;
            dest.h = clip->h;
            SDL_RenderCopy(renderer, texture, clip, &dest);
        }
        else
        {
            SDL_Rect src = {0, 0, width, height};
            SDL_RenderCopy(renderer, texture, &src, &dest);
        }
    }

    void Texture::renderTextureSimple(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip)
    {
        #ifdef DEBUG
        SDL_assert(texture != NULL);
        SDL_assert(renderer != NULL);
        #endif // DEBUG
        SDL_RenderCopy(renderer, texture, clip, &dest);
    }

}
