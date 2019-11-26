#include <memory>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "texture.h"
#include "../Core/colors.h"

using namespace std;

namespace Ossium
{

    REGISTER_COMPONENT(Texture);

    void Texture::Render(Renderer& renderer)
    {

        SDL_Rect dest = GetSDL(WorldPosition());
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
                SDL_RenderCopyEx(renderer.GetRendererSDL(), source->outlineTexture, &clip, &dest, WorldRotation().GetDegrees(), &trueOrigin, flip);
            }
            SDL_RenderCopyEx(renderer.GetRendererSDL(), source->texture, &clip, &dest, WorldRotation().GetDegrees(), &trueOrigin, flip);
        }
        else
        {
            if (source->outlineTexture != NULL)
            {
                SDL_RenderCopyEx(renderer.GetRendererSDL(), source->outlineTexture, NULL, &dest, WorldRotation().GetDegrees(), &trueOrigin, flip);
            }
            SDL_RenderCopyEx(renderer.GetRendererSDL(), source->texture, NULL, &dest, WorldRotation().GetDegrees(), &trueOrigin, flip);
        }
    }

    void Texture::Update()
    {
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
        if (GetRenderLayer() < 0)
        {
            /// Now we have an image loaded, register the texture instance so it can be rendered.
            GraphicComponent::OnLoadFinish();
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
        source.x = (source.x - WorldPosition().x + (width * 0.5f)) / (width / (float)(clip.w == 0 ? 0 : clip.w));
        source.y = (source.y - WorldPosition().y + (height * 0.5f)) / (height / (float)(clip.h == 0 ? 0 : clip.h));
        return source;
    }

}
