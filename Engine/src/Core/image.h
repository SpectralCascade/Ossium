/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
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

#include "bgfx/bgfx.h"

#include "schemamodel.h"
#include "resourcecontroller.h"
#include "renderinput.h"
#include "colors.h"

namespace Ossium
{

    // This class wraps a single SDL_Surface (an image stored in RAM)
    // and/or a single texture on the GPU.
    class OSSIUM_EDL Image : public Resource
    {
    public:
        DECLARE_RESOURCE(Image);

        Image();
        ~Image();

        /// Destroys the image, freeing it from memory. Does not modify the temporary SDL_Surface
        void Free();
        /// Frees the surface from memory, but not the GPU texture.
        void FreeSurface();

        /// Load an image and returns true if it was successful
        bool Load(std::string guid_path);

        /// Creates an empty surface; you may also specify the colour of the empty surface.
        static SDL_Surface* CreateEmptySurface(int w, int h, SDL_Color color = Colors::Transparent);

        /// Frees the current surface and sets it to this. Useful if you want to generate surfaces on the fly.
        /// NOTE: Does not make a copy of the surface data. Simply takes ownership of the reference.
        void SetSurface(SDL_Surface* loadedSurface);

    private:
        /// Sets the surface pixel format to use. Whenever SetSurface() or CreateEmptySurface() are called, this method is automatically called.
        /// Recreates the surface with the specified format if it doesn't have the same format already.
        void SetSurfaceFormat(Uint32 pixelFormat);

    public:
        /// Returns the surface pixel format. Returns SDL_PIXELFORMAT_UNKNOWN if no surface is loaded.
        Uint32 GetPixelFormat();

        // Post-load texture initialisation. You must call this method after successfully calling Load()
        // if you wish to render the image to the screen.
        bool Init(Uint64 flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

        /// Shorthand method
        bool LoadAndInit(std::string guid_path, Uint64 flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

        /// Returns true if the texture is not NULL
        /// TODO: remove me, does same job as GetTexture().
        bool Initialised();

        /// Renders the texture if pushed onto the GPU.
        void Render(
            RenderInput* pass,
            SDL_Rect dest,
            SDL_Rect* clip = NULL,
            SDL_Point* origin = NULL,
            double rotation = 0.0,
            SDL_Color modulation = Colors::White,
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
        std::string GetPathName();

        // Copies the surface data from RAM to GPU memory (creating a GPU texture).
        // Returns BGFX_INVALID_HANDLE upon failure (e.g. no surface is loaded).
        // Calling this method automatically destroys the current GPU texture if loaded.
        bgfx::TextureHandle PushGPU(Uint64 flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

        // Destroys the GPU texture if one exists.
        void PopGPU();

        // Returns the GPU texture, or BGFX_INVALID_HANDLE if not pushed onto the GPU.
        bgfx::TextureHandle GetTexture();

        // Returns the loaded surface, or NULL if not loaded.
        SDL_Surface* GetSurface();

        // Returns the texture flags.
        Uint64 GetTextureFlags();

    protected:
        NOCOPY(Image);

        /// The path used to load the current image.
        std::string pathname = "";

        /// The image prior to being pushed to the GPU as a texture
        SDL_Surface* tempSurface = NULL;

        // Handle to the GPU texture
        bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;

        // Handle to uniform associated with the GPU texture
        bgfx::UniformHandle uniform = BGFX_INVALID_HANDLE;

        // TODO move shader program somewhere shared
        bgfx::ShaderHandle shaderVertex = BGFX_INVALID_HANDLE;
        bgfx::ShaderHandle shaderFrag = BGFX_INVALID_HANDLE;
        bgfx::ProgramHandle shaderProgram = BGFX_INVALID_HANDLE;

        /// Dimensions of the GPU texture.
        int widthGPU = 0;
        int heightGPU = 0;

        /// The pixel format of the surface.
        Uint32 format = SDL_PIXELFORMAT_ARGB8888;

        /// GPU texture and sampler flags
        Uint64 flags;

        // Vertices for rendering
        // TODO 
        float vertices[4][5];
        
        // Vertex buffer object
        bgfx::VertexBufferHandle vbo;
        
        // Index buffer object
        bgfx::IndexBufferHandle ibo;

    };

}

#endif // IMAGE_H
