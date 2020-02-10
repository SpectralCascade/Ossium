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
#ifndef IMAGE_H
#define IMAGE_H

#include "schemamodel.h"
#include "resourcecontroller.h"
#include "renderer.h"
#include "colors.h"

namespace Ossium
{

    /// Forward declarations
    class Texture;
    class TexturePack;

    /// This class wraps a single SDL_Surface (an image stored in RAM) and/or a single SDL_Texture (an image usually stored in GPU memory).
    class OSSIUM_EDL Image : public Resource
    {
    public:
        DECLARE_RESOURCE(Image);

        Image() = default;
        ~Image();

        /// TODO: modify TexturePack and remove this friend statement
        friend class Ossium::TexturePack;

        /// Destroys the image, freeing it from memory. Does not modify the temporary SDL_Surface
        void Free();
        /// Frees the surface from memory, but not the GPU texture.
        void FreeSurface();

        /// Load an image and returns true if it was successful
        bool Load(string guid_path);

        /// Creates an empty surface. Optionally specify a pixel format, if not specified the last set surface pixel format will be used.
        bool CreateEmptySurface(int w, int h, Uint32 pixelFormat = SDL_PIXELFORMAT_UNKNOWN);

        /// Frees the current surface and sets it to this. Useful if you want to generate surfaces on the fly.
        /// NOTE: Does not make a copy of the surface data. Simply takes ownership of the reference.
        /// Optionally specify a pixel format, if not specified the last set surface pixel format will be used.
        void SetSurface(SDL_Surface* loadedSurface, Uint32 pixelFormat = SDL_PIXELFORMAT_UNKNOWN);

        /// Sets the surface pixel format to use. Whenever SetSurface() or CreateEmptySurface() are called, this method is automatically called.
        /// Recreates the surface with the specified format if it doesn't have the same format already.
        void SetSurfaceFormat(Uint32 pixelFormat);

        /// Returns the surface pixel format. Returns SDL_PIXELFORMAT_UNKNOWN if no surface is loaded.
        Uint32 GetSurfacePixelFormat();

        /// Returns the texture pixel format. Returns SDL_PIXELFORMAT_UNKNOWN if no texture is created.
        /// Note the only way to set the texture format is to call SetSurfaceFormat() and then call PushGPU() again.
        Uint32 GetTexturePixelFormat();

        /// Post-load texture initialisation; pass the window pixel format if you wish to manipulate pixel data.
        /// You MUST call this method after successfully calling Load() if you wish to render the image to the screen.
        /// If cache is set to true, the original image is stored in memory as an SDL_Surface for quick initialisation without loading the image again
        /// (in addition to the modifiable SDL_Texture in video memory)! This could be useful if you want to remove all applied effects on the image frequently,
        /// as long as your target system has adequate memory capacity for storing the original image. Leave this set to false unless you know what you're doing!
        bool Init(Renderer& renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_ARGB8888, int accessMode = SDL_TEXTUREACCESS_STATIC);

        /// Shorthand method
        bool LoadAndInit(string guid_path, Renderer& renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_ARGB8888, int accessMode = SDL_TEXTUREACCESS_STATIC);

        /// Returns true if the texture is not NULL
        /// TODO: remove me, does same job as GetTexture().
        bool Initialised();

        /// Renders the GPU texture if loaded (call PushGPU() first).
        void Render(
            SDL_Renderer* renderer,
            SDL_Rect dest,
            SDL_Rect* clip = NULL,
            SDL_Point* origin = NULL,
            double rotation = 0.0,
            SDL_Color modulation = Colors::WHITE,
            SDL_BlendMode blending = SDL_BLENDMODE_BLEND,
            SDL_RendererFlip flip = SDL_FLIP_NONE
        );

        // TODO: blitting method for surfaces. Not strictly essential however, can just use SDL directly for now

        /// Returns the width of the image in GPU memory, or if not loaded in GPU memory, returns the width of the surface.
        int GetWidth();
        /// Returns the height of the image, or if not loaded in GPU memory, returns the height of the surface.
        int GetHeight();

        /// Explicit version of GetWidth(), but returns 0 if not loaded in GPU memory (even if the surface is loaded).
        int GetWidthGPU();
        /// Explicit version of GetHeight(), but returns 0 if not loaded in GPU memory (even if the surface is loaded).
        int GetHeightGPU();

        /// Returns the width of the surface. Returns 0 if no surface is loaded.
        int GetWidthSurface();
        /// Returns the height of the surface. Returns 0 if no surface is loaded.
        int GetHeightSurface();

        /// Returns the name of the loaded image, if it has one.
        string GetPathName();

        /// Applies a pixel manipulation function to the image. The argument function accepts pixel data as an SDL_Color
        /// in addition to an SDL_Point which indicates which pixel is being modified relative to the top left of the image.
        /// It works by iterating over each pixel in the source image and giving your function relevant information. The function must return an SDL_Color value which is what the pixel
        /// will be set to. Note that the image must be initialised with the window's pixel format to apply pixel manipulation effects!
        /// Also note that this doesn't do it's work on the GPU, so be wary of using it frequently as it's pretty expensive
        /// Returns false on failure.
        /// TODO: pass std::function object instead of using template
        template<class Func>
        bool ApplyEffect(Func f, SDL_Rect* clipArea = nullptr)
        {
            if (LockPixels())
            {
                Uint32* pixelArray = reinterpret_cast<Uint32*>(pixels);
                SDL_PixelFormat* pixelFormat = SDL_AllocFormat(format);
                if (pixelFormat == NULL)
                {
                    Logger::EngineLog().Error("Could not allocate pixel format to apply effect! SDL_Error: {0}", SDL_GetError());
                    return false;
                }
                SDL_Color pixelData;
                SDL_Point pixelPos;
                if (clipArea == nullptr)
                {
                    for (int i = 0, counti = (pitch / 4) * heightGPU; i < counti; i++)
                    {
                        pixelData = ConvertToColor(pixelArray[i], pixelFormat);
                        pixelPos.x = i % widthGPU;
                        pixelPos.y = i / widthGPU;
                        SDL_Color outputColor = f(pixelData, pixelPos);
                        pixelArray[i] = SDL_MapRGBA(pixelFormat, outputColor.r, outputColor.g, outputColor.b, outputColor.a);
                    }
                }
                else
                {
                    int memwidth = (pitch / 4);
                    int clipskipy = (memwidth * clipArea->y);
                    int clipxw = clipArea->w + clipArea->x;
                    int increment = (memwidth - clipArea->w);
                    for (int i = clipskipy + clipArea->x, counti = memwidth * clipArea->h; i < counti; i++)
                    {
                        if (i >= clipxw + clipskipy)
                        {
                            i += increment;
                            clipskipy += memwidth;
                            if (i > counti)
                            {
                                break;
                            }
                        }
                        pixelData = ConvertToColor(pixelArray[i], pixelFormat);
                        pixelPos.x = i % widthGPU;
                        pixelPos.y = i / widthGPU;
                        SDL_Color outputColor = f(pixelData, pixelPos);
                        pixelArray[i] = SDL_MapRGBA(pixelFormat, outputColor.r, outputColor.g, outputColor.b, outputColor.a);
                    }
                }
                UnlockPixels();
                SDL_FreeFormat(pixelFormat);
                return true;
            }
            return false;
        };

        /// Copies the surface data from RAM to GPU memory (creating a GPU texture). Returns NULL upon failure (e.g. no surface is loaded).
        /// Calling this method automatically destroys the current GPU texture if loaded.
        SDL_Texture* PushGPU(Renderer& renderer, int accessMode = SDL_TEXTUREACCESS_STREAMING);

        /// Destroys the GPU texture if one .
        void PopGPU();

        /// Returns the GPU texture, or NULL if not pushed onto the GPU.
        SDL_Texture* GetTexture();

        /// Returns the loaded surface, or NULL if not loaded.
        SDL_Surface* GetSurface();

        /// Locks the texture on the GPU so the raw pixels may be modified
        bool LockPixels();

        /// When locked, returns pixels from the GPU texture.
        void* GetPixels();

        /// When locked, returns the length of the pixel data in bytes.
        int GetPitch();

        /// Returns the texture access mode (whether the texture can be streamed or rendered upon or not).
        int GetTextureAccessMode();

        /// Unlocks the texture on the GPU so the raw pixels can no longer be modified
        bool UnlockPixels();

    protected:
        NOCOPY(Image);

        /// The path used to load the current image.
        string pathname = "";

        /// The image prior to being converted to an SDL_Texture
        SDL_Surface* tempSurface = NULL;

        /// A representation of the image in video memory
        SDL_Texture* texture = NULL;

        /// Dimensions of the GPU texture.
        int widthGPU = 0;
        int heightGPU = 0;

        /// The pixel format of the surface.
        Uint32 format = SDL_PIXELFORMAT_ARGB8888;

        /// The pixel format of the texture.
        Uint32 textureFormat = SDL_PIXELFORMAT_UNKNOWN;

        /// The access mode used for the current GPU texture.
        int access = -1;

        /// Pixel data; NULL unless you have locked the pixels in GPU memory.
        void* pixels = NULL;

        /// Width (length of each line) of the image as laid out in GPU memory, in bytes.
        int pitch = 0;

    };

}

#endif // IMAGE_H
