#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "oss_texture.h"

using namespace std;

OSS_Texture::OSS_Texture()
{
    texture = NULL;
    width = 0;
    height = 0;
    pixels = NULL;
    pitch = 0;
}

OSS_Texture::~OSS_Texture()
{
    freeTexture();
}

void OSS_Texture::freeTexture()
{
    if (texture != NULL)
    {
        unlockPixels();
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
}

bool OSS_Texture::loadImage(string path, SDL_Renderer* renderer, Uint32 windowPixelFormat)
{
    freeTexture();
    SDL_Surface* loadedSurface = NULL;
    SDL_Texture* newTexture = NULL;
    loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface != NULL)
    {
        if (windowPixelFormat != SDL_PIXELFORMAT_UNKNOWN)
        {
            SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, windowPixelFormat, 0);
            if (formattedSurface == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
            }
            else
            {
                newTexture = SDL_CreateTexture(renderer, windowPixelFormat, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
                if (newTexture == NULL)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
                }
                else
                {
                    /// Grab pixel manipulation data
                    SDL_LockTexture(newTexture, NULL, &pixels, &pitch);
                    memcpy(pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);
                    SDL_UnlockTexture(newTexture);
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
            newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
            if (newTexture == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
            }
            else
            {
                width = loadedSurface->w;
                height = loadedSurface->h;
            }
        }
        SDL_FreeSurface(loadedSurface);
        loadedSurface = NULL;
    }
    texture = newTexture;
    return texture != NULL;
}

bool OSS_Texture::createText(string text, SDL_Renderer* renderer, TTF_Font* font, SDL_Color color)
{
    freeTexture();
    SDL_Texture* newTexture = NULL;
    SDL_Surface* textSurface = NULL;
    textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface == NULL)
    {
        newTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
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

void OSS_Texture::setBlendMode(SDL_BlendMode blend)
{
    SDL_SetTextureBlendMode(texture, blend);
}

void OSS_Texture::setAlpha(Uint8 a)
{
    SDL_SetTextureAlphaMod(texture, a);
}

void OSS_Texture::setColorMod(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(texture, r, g, b);
}

void OSS_Texture::render(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
{
    if (clip != NULL)
    {
        SDL_RenderCopyEx(renderer, texture, clip, &dest, angle, origin, flip);
    }
    else
    {
        SDL_Rect src = {0, 0, width, height};
        SDL_RenderCopyEx(renderer, texture, &src, &dest, angle, origin, flip);
    }
}

void OSS_Texture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip)
{
    SDL_Rect dest = {x, y, width, height};
    if (clip != NULL)
    {
        SDL_RenderCopy(renderer, texture, clip, &dest);
    }
    else
    {
        SDL_Rect src = {0, 0, width, height};
        SDL_RenderCopy(renderer, texture, &src, &dest);
    }
}

int OSS_Texture::getWidth()
{
    return width;
}

int OSS_Texture::getHeight()
{
    return height;
}
/*
void OSS_Texture::setWidth(int w)
{
    width = w;
}

void OSS_Texture::setHeight(int h)
{
    height = h;
}
*/
bool OSS_Texture::lockPixels()
{
    bool success = true;
    if (pixels != NULL)
    {
        success = false;
    }
    else
    {
        if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0)
        {
            success = false;
        }
    }
    return success;
}

bool OSS_Texture::unlockPixels()
{
    bool success = true;
    if (pixels != NULL)
    {
        success = false;
    }
    else
    {
        SDL_UnlockTexture(texture);
        pixels = NULL;
        pitch = 0;
    }
    return success;
}
