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
extern "C"
{
    #include <SDL_image.h>
}

#include "image.h"
#include "coremaths.h"
#include "shader.h"

using namespace std;

namespace Ossium
{

    REGISTER_RESOURCE(Image);

    Image::Image()
    {
        // Load shaders
        shaderVertex.LoadAndInit(Shader::GetPath("image.vert"));
        shaderFragment.LoadAndInit(Shader::GetPath("image.frag"));
        shaderProgram = bgfx::createProgram(shaderVertex.GetHandle(), shaderFragment.GetHandle(), false);
    }

    Image::~Image()
    {
        PopGPU();
        FreeSurface();

        bgfx::destroy(shaderProgram);
    }

    void Image::FreeSurface()
    {
        if (tempSurface != NULL)
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = NULL;
        }
        format = SDL_PIXELFORMAT_UNKNOWN;
    }

    bool Image::Load(string guid_path)
    {
        FreeSurface();
        #ifdef SDL_IMAGE_H_
        tempSurface = IMG_Load(guid_path.c_str());
        if (tempSurface == NULL)
        {
            Log.Error("Could not load image '{0}'! IMG_Error: {1}", guid_path, IMG_GetError());
        }
        #else
        tempSurface = SDL_LoadBMP(guid_path.c_str());
        if (tempSurface == NULL)
        {
            Log.Error("Could not load image '{0}'! SDL_Error: {1}", guid_path, SDL_GetError());
        }
        #endif // SDL_IMAGE_H_
        else
        {
            pathname = guid_path;
            format = SDL_MasksToPixelFormatEnum(
                tempSurface->format->BitsPerPixel,
                tempSurface->format->Rmask,
                tempSurface->format->Gmask,
                tempSurface->format->Bmask,
                tempSurface->format->Amask
            );
            // Use this format by default
            SetSurfaceFormat(IMAGE_FORMAT);
        }
        
        return tempSurface != NULL;
    }

    SDL_Surface* Image::CreateEmptySurface(int w, int h, SDL_Color color)
    {
        auto format = IMAGE_FORMAT;
        SDL_Surface* emptySurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, format);
        if (emptySurface != NULL && color != Colors::Transparent)
        {
            SDL_Rect rect = {0, 0, w, h};
            SDL_PixelFormat* allocatedFormat = SDL_AllocFormat(format);
            if (allocatedFormat != NULL)
            {
                SDL_FillRect(
                    emptySurface,
                    &rect,
                    SDL_MapRGBA(allocatedFormat, color.r, color.g, color.b, color.a)
                );
                SDL_FreeFormat(allocatedFormat);
            }
        }
        return emptySurface;
    }

    void Image::SetSurface(SDL_Surface* loadedSurface)
    {
        FreeSurface();
        tempSurface = loadedSurface;
        format = SDL_MasksToPixelFormatEnum(
            tempSurface->format->BitsPerPixel,
            tempSurface->format->Rmask,
            tempSurface->format->Gmask,
            tempSurface->format->Bmask,
            tempSurface->format->Amask
        );
        SetSurfaceFormat(IMAGE_FORMAT);
    }

    void Image::SetSurfaceFormat(Uint32 pixelFormat)
    {
        if (format != pixelFormat)
        {
            if (pixelFormat == SDL_PIXELFORMAT_UNKNOWN)
            {
                Log.Warning("Cannot set surface format to unknown surface format!");
                return;
            }
            if (tempSurface != NULL && tempSurface->format->format != pixelFormat)
            {
                SDL_Surface* formatted = NULL;
                formatted = SDL_ConvertSurfaceFormat(tempSurface, pixelFormat, 0);
                if (formatted != NULL)
                {
                    SDL_FreeSurface(tempSurface);
                    tempSurface = formatted;
                }
                else
                {
                    Log.Error("Failed to convert surface format from {0} to {1}!", SDL_GetPixelFormatName(format), SDL_GetPixelFormatName(pixelFormat));
                }
            }
            format = pixelFormat;
        }
    }

    Uint32 Image::GetPixelFormat()
    {
        return format;
    }

    bool Image::Init(Uint64 flags)
    {
        PopGPU();
        bool success = PushGPU(flags).idx != bgfx::kInvalidHandle;
        FreeSurface();
        return success;
    }

    bool Image::LoadAndInit(string guid_path, Uint64 flags)
    {
        return Load(guid_path) && Init(flags);
    }

    bool Image::Initialised()
    {
        return texture.idx != bgfx::kInvalidHandle;
    }

    struct ImageVertex
    {
        float x;
        float y;
        float u;
        float v;
        Uint32 color;

        static bgfx::VertexLayout Layout()
        {
            bgfx::VertexLayout layout;
            layout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
            return layout;
        }

    };

    void Image::Render(
        RenderInput* pass,
        SDL_Rect dest,
        SDL_Rect* clip,
        SDL_Point* origin,
        double rotation,
        SDL_Color modulation,
        SDL_BlendMode blending,
        SDL_RendererFlip flip)
    {
        if (!bgfx::isValid(texture))
        {
            // Draw error
            // TODO draw a tiled error texture?
            //Log.Error("Texture is invalid, can't render!");
            Rect(dest).DrawFilled(pass, Color(255, 100, 255, 255));
            return;
        }

        // TODO setup these texture effects
        //SDL_SetTextureBlendMode(texture, blending);
        //SDL_SetTextureColorMod(texture, modulation.r, modulation.g, modulation.b);
        //SDL_SetTextureAlphaMod(texture, modulation.a);

        Renderer* renderer = pass->GetRenderer();
        
        // Vertices for rendering
        Uint32 color_mod = ColorToUint32(modulation, SDL_PIXELFORMAT_RGBA32);
        ImageVertex vertices[] = {
            {(float)dest.x, (float)dest.y, 0, 0, color_mod},
            {(float)dest.x, (float)dest.y + dest.h, 0, 1, color_mod},
            {(float)dest.x + dest.w, (float)dest.y + dest.h, 1, 1, color_mod},
            {(float)dest.x + dest.w, (float)dest.y, 1, 0, color_mod}
        };

        // Create index array (quad formed of two triangles)
        uint16_t indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        // Set primitive type to triangles
        renderer->SetState(0
            // Write colour
            | BGFX_STATE_WRITE_RGB
            // Write alpha
            | BGFX_STATE_WRITE_A
            // Alpha opacity blending
            | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_DST_ALPHA)
            //| BGFX_STATE_BLEND_EQUATION_SEPARATE(BGFX_STATE_BLEND_EQUATION_ADD, BGFX_STATE_BLEND_EQUATION_MAX)
        );
        renderer->UpdateStateAndColor();

        // Vertex buffer object
        bgfx::VertexBufferHandle vbo = bgfx::createVertexBuffer(
            bgfx::copy(vertices, sizeof(vertices)), ImageVertex::Layout()
        );
        // Index buffer object
        bgfx::IndexBufferHandle ibo = bgfx::createIndexBuffer(bgfx::copy(indices, sizeof(indices)));

        if (clip && clip->w > 0 && clip->h > 0)
        {
            // Modify UV coords to implement texture clipping
            for (unsigned int i = 0, counti = 4; i < counti; i++)
            {
                vertices[i].u = ((widthGPU - clip->x) / widthGPU) + (clip->w / widthGPU) * vertices[i].u;
                vertices[i].v = ((heightGPU - clip->y) / heightGPU) + (clip->h / heightGPU) * vertices[i].v;
            }
        }

        Matrix<4, 4> view = Matrix<4, 4>::Identity();
        Matrix<4, 4> proj = Matrix<4, 4>::Orthographic(
            0,
            renderer->GetWidth(),
            renderer->GetHeight(),
            0,
            0,
            100
        );

        bgfx::setViewTransform(pass->GetID(), &view, &proj);
        
        bgfx::setVertexBuffer(0, vbo);
        bgfx::setIndexBuffer(ibo);
        // Set the first (and only) uniform - stage 0 - to the created sampler
        bgfx::setTexture(0, uniform, texture);

        // Submit the draw call
        bgfx::submit(pass->GetID(), shaderProgram);

        bgfx::destroy(vbo);
        bgfx::destroy(ibo);
    }

    int Image::GetWidth()
    {
        return GetTexture().idx != bgfx::kInvalidHandle ? GetWidthGPU() : GetWidthSurface();
    }

    int Image::GetHeight()
    {
        return GetTexture().idx != bgfx::kInvalidHandle ? GetHeightGPU() : GetHeightSurface();
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

    bgfx::TextureHandle Image::GetTexture()
    {
        return texture;
    }

    SDL_Surface* Image::GetSurface()
    {
        return tempSurface;
    }

    bgfx::TextureHandle Image::PushGPU(Uint64 flags)
    {
        // Free GPU memory if in use
        PopGPU();

        // Set the texture flags
        this->flags = flags;

        if (tempSurface == NULL)
        {
            Log.Error("No surface loaded, cannot copy to GPU memory!");
            texture = BGFX_INVALID_HANDLE;
        }
        else
        {
            uint32_t tex_size = tempSurface->pitch * tempSurface->h;
            Log.Info("Creating 2D texture for the GPU ({0} bytes)", tex_size);
            texture = bgfx::createTexture2D(
                tempSurface->w,
                tempSurface->h,
                false,
                1,
                bgfx::TextureFormat::RGBA8,
                flags,
                bgfx::copy(tempSurface->pixels, tex_size)
            );
            if (texture.idx == bgfx::kInvalidHandle)
            {
                Log.Error("Failed to create GPU texture from surface! Unknown BGFX error.");
                widthGPU = 0;
                heightGPU = 0;
            }
            else
            {
                widthGPU = tempSurface->w;
                heightGPU = tempSurface->h;
                uniform = bgfx::createUniform("tex0", bgfx::UniformType::Sampler);
            }
        }
        
        return texture;
    }

    void Image::PopGPU()
    {
        if (texture.idx != bgfx::kInvalidHandle)
        {
            Log.Info("Destroying GPU texture...");
            bgfx::destroy(texture);
            texture = BGFX_INVALID_HANDLE;
        }
        if (uniform.idx != bgfx::kInvalidHandle)
        {
            bgfx::destroy(uniform);
            uniform = BGFX_INVALID_HANDLE;
        }
        widthGPU = 0;
        heightGPU = 0;
    }

    Uint64 Image::GetTextureFlags()
    {
        return flags;
    }

}
