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
#include <memory>
#include <string>

#include "texture.h"
#include "../Core/colors.h"

using namespace std;

namespace Ossium
{

    REGISTER_COMPONENT(Texture);

    void Texture::Render(Renderer& renderer)
    {

        SDL_Rect dest = GetSDL(GetTransform()->GetWorldPosition(), GetTransform()->GetWorldScale());
        if (source == nullptr || source->GetTexture() == NULL)
        {
            SDL_SetRenderDrawColor(renderer.GetRendererSDL(), 255, 100, 255, 255);
            SDL_RenderFillRect(renderer.GetRendererSDL(), &dest);
            return;
        }

        SDL_Point trueOrigin = {(int)(origin.x * width * GetTransform()->GetWorldScale().x), (int)(origin.y * height * GetTransform()->GetWorldScale().y)};

        /// Rendering time!
        if (clip.w > 0 && clip.h > 0)
        {
            source->Render(renderer.GetRendererSDL(), dest, &clip, &trueOrigin, GetTransform()->GetWorldRotation().GetDegrees(), modulation, blending, flip);
        }
        else
        {
            source->Render(renderer.GetRendererSDL(), dest, NULL, &trueOrigin, GetTransform()->GetWorldRotation().GetDegrees(), modulation, blending, flip);
        }
    }

    void Texture::OnLoadFinish()
    {
        GraphicComponent::OnLoadFinish();
        if (source != nullptr && source->Initialised() && imgPath == source->GetPathName())
        {
            return;
        }
        /// Don't configure dimensions, they should be specified by the schema data.
        SetSource(GetService<ResourceController>()->Get<Image>(imgPath, *GetService<Renderer>()), false);
        Log.Verbose("Loaded texture with source from \"{0}\"", imgPath);
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
                width = src->GetWidth();
                height = src->GetHeight();
                clip.w = width;
                clip.h = height;
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
        if (GetRenderLayer() < 0)
        {
            /// Now we have an image loaded, register the texture instance so it can be rendered.
            GraphicComponent::OnLoadFinish();
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
    void Texture::SetColorMod(Uint8 r, Uint8 g, Uint8 b)
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
        return source->GetWidth();
    }
    int Texture::GetSourceHeight()
    {
        return source->GetHeight();
    }
    SDL_Rect Texture::GetClip()
    {
        return clip;
    }

    Point Texture::ScreenToLocalPoint(Point source)
    {
        source.x = (source.x - GetTransform()->GetWorldPosition().x + (width * 0.5f)) / (width / (float)(clip.w == 0 ? 0 : clip.w));
        source.y = (source.y - GetTransform()->GetWorldPosition().y + (height * 0.5f)) / (height / (float)(clip.h == 0 ? 0 : clip.h));
        return source;
    }

}
