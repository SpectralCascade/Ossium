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
    #include <SDL_image.h>
}

#include "image.h"

namespace Ossium
{

    REGISTER_RESOURCE(Image);

    Image::~Image()
    {
        PopGPU();
        FreeSurface();
    }

    void Image::FreeSurface()
    {
        if (tempSurface != NULL)
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }
    }

    bool Image::Load(string guid_path)
    {
        FreeSurface();
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

    bool Image::CreateEmptySurface(int w, int h, Uint32 pixelFormat)
    {
        FreeSurface();
        tempSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, pixelFormat);
        if (tempSurface == NULL)
        {
            return false;
        }
        return true;
    }

    void Image::SetSurface(SDL_Surface* loadedSurface)
    {
        FreeSurface();
        tempSurface = loadedSurface;
    }

    bool Image::Init(Renderer& renderer, Uint32 pixelFormatting, int accessMode)
    {
        PopGPU();
        bool success = PushGPU(renderer, pixelFormatting, accessMode) != NULL;
        FreeSurface();
        return success;
    }

    bool Image::LoadAndInit(string guid_path, Renderer& renderer, Uint32 pixelFormatting, int accessMode)
    {
        return Load(guid_path) && Init(renderer, pixelFormatting, accessMode);
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

        SDL_SetTextureBlendMode(texture, blending);
        SDL_SetTextureColorMod(texture, modulation.r, modulation.g, modulation.b);
        SDL_SetTextureAlphaMod(texture, modulation.a);

        /// Rendering time!
        if (clip && clip->w > 0 && clip->h > 0)
        {
            SDL_RenderCopyEx(renderer, texture, clip, &dest, rotation, origin, flip);
        }
        else
        {
            SDL_RenderCopyEx(renderer, texture, NULL, &dest, rotation, origin, flip);
        }
    }

    int Image::GetWidth()
    {
        return GetTexture() != NULL ? GetWidthGPU() : GetWidthSurface();
    }

    int Image::GetHeight()
    {
        return GetTexture() != NULL ? GetHeightGPU() : GetHeightSurface();
    }

    int Image::GetWidthGPU()
    {
        return widthGPU;
    }

    int Image::GetHeightGPU()
    {
        return heightGPU;
    }

    int Image::GetWidthSurface()
    {
        return tempSurface != NULL ? tempSurface->w : 0;
    }

    int Image::GetHeightSurface()
    {
        return tempSurface != NULL ? tempSurface->h : 0;
    }

    string Image::GetPathName()
    {
        return pathname;
    }

    SDL_Texture* Image::GetTexture()
    {
        return texture;
    }

    SDL_Surface* Image::GetSurface()
    {
        return tempSurface;
    }

    SDL_Texture* Image::PushGPU(Renderer& renderer, Uint32 pixelFormatting, int accessMode)
    {
        // Free GPU memory
        PopGPU();

        // Set the formatting and access mode
        format = pixelFormatting;
        access = accessMode;

        if (tempSurface == NULL)
        {
            Logger::EngineLog().Error("No surface loaded, cannot copy to GPU memory!");
        }
        else if (pixelFormatting != SDL_PIXELFORMAT_UNKNOWN && accessMode != SDL_TEXTUREACCESS_STATIC)
        {
            SDL_Surface* formattedSurface = NULL;
            formattedSurface = SDL_ConvertSurfaceFormat(tempSurface, pixelFormatting, 0);
            if (formattedSurface == NULL)
            {
                Logger::EngineLog().Error("Failed to format surface! SDL_Error: {0}", SDL_GetError());
            }
            else
            {
                texture = SDL_CreateTexture(renderer.GetRendererSDL(), pixelFormatting, accessMode, formattedSurface->w, formattedSurface->h);
                if (texture == NULL)
                {
                    Logger::EngineLog().Error("Failed to create GPU texture from surface! SDL_Error: {0}", SDL_GetError());
                }
                else if (accessMode == SDL_TEXTUREACCESS_STREAMING)
                {
                    // Copy surface pixels to texture
                    SDL_LockTexture(texture, NULL, &pixels, &pitch);
                    memcpy(pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);
                    SDL_UnlockTexture(texture);
                }
                pixels = NULL;
                widthGPU = formattedSurface->w;
                heightGPU = formattedSurface->h;

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
                widthGPU = tempSurface->w;
                heightGPU = tempSurface->h;
            }
        }
        // In error case, set default values
        if (texture == NULL)
        {
            format = SDL_PIXELFORMAT_UNKNOWN;
            access = -1;
        }
        return texture;
    }

    void Image::PopGPU()
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
        format = SDL_PIXELFORMAT_UNKNOWN;
        access = -1;
        widthGPU = 0;
        heightGPU = 0;
    }

    void* Image::GetPixels()
    {
        return pixels;
    }

    int Image::GetPitch()
    {
        return pitch;
    }

    Uint32 Image::GetTexturePixelFormat()
    {
        return format;
    }

    int Image::GetTextureAccessMode()
    {
        return access;
    }

    bool Image::LockPixels()
    {
        if (texture == NULL)
        {
            return false;
        }
        else if (pixels == NULL && SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0)
        {
            Logger::EngineLog().Error("Failed to lock GPU texture! {0}", SDL_GetError());
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
