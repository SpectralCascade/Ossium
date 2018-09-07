#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "oss_texture.h"

using namespace std;

OSS_Texture::OSS_Texture()
{
    tempSurface = NULL;
    texture = NULL;
    width = 0;
    height = 0;
    pixels = NULL;
    pitch = 0;
}

OSS_Texture::~OSS_Texture()
{
    freeTexture();
    if (tempSurface != NULL)
    {
        SDL_FreeSurface(tempSurface);
        tempSurface = NULL;
    }
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

bool OSS_Texture::load(string guid_path)
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

bool OSS_Texture::postLoadInit(SDL_Renderer* renderer, Uint32 windowPixelFormat)
{
    SDL_assert(renderer != NULL);
    freeTexture();
    if (tempSurface == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "NULL surface image, cannot initialise texture!");
    }
    else if (windowPixelFormat != SDL_PIXELFORMAT_UNKNOWN)
    {
        SDL_Surface* formattedSurface = NULL;
        formattedSurface = SDL_ConvertSurfaceFormat(tempSurface, windowPixelFormat, 0);
        SDL_FreeSurface(tempSurface);
        tempSurface = NULL;
        if (formattedSurface == NULL)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to format surface! SDL_Error: %s", SDL_GetError());
        }
        else
        {
            texture = SDL_CreateTextureFromSurface(renderer, formattedSurface);
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
        texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
        if (texture == NULL)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface! SDL_Error: %s", SDL_GetError());
        }
        else
        {
            width = tempSurface->w;
            height = tempSurface->h;
        }
        SDL_FreeSurface(tempSurface);
        tempSurface = NULL;
    }
    return texture != NULL;
}

#ifdef _SDL_TTF_H
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
#endif // _SDL_TTF_H

void OSS_Texture::setBlendMode(SDL_BlendMode blend)
{
    SDL_assert(texture != NULL);
    SDL_SetTextureBlendMode(texture, blend);
}

void OSS_Texture::setAlpha(Uint8 a)
{
    SDL_assert(texture != NULL);
    SDL_SetTextureAlphaMod(texture, a);
}

void OSS_Texture::setColorMod(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_assert(texture != NULL);
    SDL_SetTextureColorMod(texture, r, g, b);
}

void OSS_Texture::render(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
{
    SDL_assert(texture != NULL);
    SDL_assert(renderer != NULL);
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

void OSS_Texture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip, float angle, SDL_Point* origin, SDL_RendererFlip flip)
{
    render(renderer, {x, y, width, height}, clip, angle, origin, flip);
}

void OSS_Texture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip)
{
    SDL_assert(texture != NULL);
    SDL_assert(renderer != NULL);
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

bool OSS_Texture::lockPixels()
{
    SDL_assert(texture != NULL);
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
    SDL_assert(texture != NULL);
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
