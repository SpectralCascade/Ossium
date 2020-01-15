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
extern "C" {
    #include <SDL2/SDL_image.h>
}

#include "utf8.h"
#include "funcutils.h"
#include "texturepack.h"
#include "jsondata.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

using namespace std;

namespace Ossium
{

    ImportedTextureData::ImportedTextureData(string rawPath, SDL_Rect clip, bool hasMipmaps, Image* tex)
    {
        path = rawPath;
        pureClip = clip;
        mipmapped = hasMipmaps;
        texture = tex;
    }

    REGISTER_RESOURCE(TexturePack);

    SDL_Rect GetMipMapClip(SDL_Rect src, int level, Uint16 minSize)
    {
        SDL_Rect mipMapClip = {src.x + src.w, src.y, src.w / 2, src.h / 2};
        int offset = src.h;
        int width = mipMapClip.w;
        for (int i = 1; i <= level; i++)
        {
            if (mipMapClip.w <= minSize || mipMapClip.h <= minSize)
            {
                break;
            }
            offset = offset / 2;
            width = width / 2;
            mipMapClip.y += offset;
            mipMapClip.w -= width;
            mipMapClip.h -= offset / 2;
        }
        return mipMapClip;
    }

    TexturePack::~TexturePack()
    {
        FreeAll();
    }

    void TexturePack::FreeAll()
    {
        FreePack();
        FreeImported();
    }

    void TexturePack::FreePack()
    {
        packData.clear();
        packedTexture.Free();
    }

    void TexturePack::FreeImported()
    {
        for (int i = 0, counti = importedData.empty() ? 0 : importedData.size(); i < counti; i++)
        {
            if (importedData[i].texture != NULL)
            {
                delete importedData[i].texture;
                importedData[i].texture = NULL;
            }
        }
        importedData.clear();
    }

    bool TexturePack::Load(string path)
    {
        FreePack();
        bool success = true;
        // Load the meta data (as schema JSON).
        // TODO: path should include file extension rather than assuming
        JSON data;
        if (data.Import(path + ".tpm"))
        {
            SerialiseIn(data);
        }
        else
        {
            Logger::EngineLog().Warning("Failed to import TexturePack data.");
            success = false;
        }
        // Now load the texture itself
        // TODO: image type and path could be specified in the schema to allow other image formats?
        return success && packedTexture.Load(path + ".png");
    }

    bool TexturePack::Init(Renderer& renderer, Uint32 pixelFormatting)
    {
        return packedTexture.Init(renderer, pixelFormatting);
    }

    bool TexturePack::Import(string path, Renderer& renderer, Uint32 pixelFormatting, int minMipMapSize)
    {
        Image* importedTexture = new Image();
        if (!importedTexture->LoadAndInit(path, renderer, pixelFormatting))
        {
            Logger::EngineLog().Error("Failed to import texture into texture pack with path '{0}'", path);
            return false;
        }
        bool mipmapped = false;
        SDL_Rect src = {0, 0, importedTexture->GetWidth(), importedTexture->GetHeight()};
        // If a pixel format is provided, attempt to generate a mipmap for the imported texture
        if (pixelFormatting != SDL_PIXELFORMAT_UNKNOWN && minMipMapSize >= 0)
        {
            SDL_Texture* mipmappedTexture = NULL;
            SDL_Renderer* tarGetRendererSDL = renderer.GetRendererSDL();
            mipmappedTexture = SDL_CreateTexture(tarGetRendererSDL, pixelFormatting, SDL_TEXTUREACCESS_TARGET, (src.w / 2) * 3, src.h);
            // Cache original renderer target
            SDL_Texture* originalTarget = SDL_GetRenderTarget(tarGetRendererSDL);
            // Configure the renderer so it renders to the mipmapped texture
            SDL_SetRenderTarget(tarGetRendererSDL, mipmappedTexture);
            SDL_RenderClear(tarGetRendererSDL);
            // Render the imported texture to the mipmapped texture
            SDL_RenderCopy(tarGetRendererSDL, importedTexture->texture, NULL, &src);
            // Now obtain mipmap levels and render the mipmap to texture
            SDL_Rect previousClip = {0, 0, 0, 0};
            int level = 0;
            while (true)
            {
                SDL_Rect mipmapClip = GetMipMapClip(src, level, minMipMapSize);
                if (previousClip.h == mipmapClip.h)
                {
                    break;
                }
                SDL_RenderCopy(tarGetRendererSDL, importedTexture->texture, &src, &mipmapClip);
                level++;
                previousClip = mipmapClip;
            }
            SDL_RenderPresent(tarGetRendererSDL);
            SDL_Rect targetRect = {0, 0, (src.w / 2) * 3, src.h};
            SDL_Surface* renderSurface = SDL_CreateRGBSurface(0, targetRect.w, targetRect.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            SDL_RenderReadPixels(tarGetRendererSDL, &targetRect, SDL_PIXELFORMAT_ARGB8888, renderSurface->pixels, renderSurface->pitch);
            mipmappedTexture = SDL_CreateTextureFromSurface(tarGetRendererSDL, renderSurface);
            // Free the original imported texture and replace with mipmapped version
            SDL_DestroyTexture(importedTexture->texture);
            importedTexture->texture = mipmappedTexture;
            importedTexture->width = renderSurface->w;
            importedTexture->height = renderSurface->h;

            SDL_FreeSurface(renderSurface);
            // Reset the render target of the renderer
            SDL_SetRenderTarget(tarGetRendererSDL, originalTarget);
            mipmapped = true;
        }
        importedData.push_back(ImportedTextureData(path, src, mipmapped, importedTexture));
        return true;
    }

    bool TexturePack::ImportFont(string path, int pointSize, Renderer& renderer, string charset, int style, long index, Uint32 pixelFormatting)
    {
        TTF_Font* loaded = TTF_OpenFontIndex(path.c_str(), pointSize, index);
        if (loaded != NULL)
        {
            // TODO: extract font name?
            string fontName = path;

            // Used to extract UTF8 characters
            string utfChar = "";
            Uint8 utfBytes = 0;
            for (unsigned int i = 0, counti = charset.length(); i < counti; i++)
            {
                Uint8 meta = CheckUTF8((Uint8)charset[i]);
                if (meta)
                {
                    // UTF-8 encoded byte
                    if (meta > 1)
                    {
                        utfBytes = meta;
                    }
                    utfChar += charset[i];
                    utfBytes--;
                    if (utfBytes <= 0)
                    {
                        CreateFromUTF8(fontName, pointSize, utfChar, loaded, renderer, pixelFormatting, style);
                        utfChar.clear();
                    }
                }
                else
                {
                    // Plain old ASCII
                    CreateFromUTF8(fontName, pointSize, string() + charset[i], loaded, renderer, pixelFormatting, style);
                }
            }
        }
        else
        {
            Logger::EngineLog().Error("Failed to load font '{0}' into texture packer! TTF_Error: {1}", path, TTF_GetError());
            return false;
        }
        return true;
    }

    bool TexturePack::CreateFromUTF8(string fontName, int ptSize, string utfChar, TTF_Font* font, Renderer& renderer, Uint32 pixelFormatting, int style)
    {
        Image* texture = new Image();
        if (!texture->CreateFromText(renderer, font, utfChar, Colors::WHITE, TTF_HINTING_NONE, 0, 0, style, RENDERTEXT_BLEND, Colors::BLACK))
        {
            Logger::EngineLog().Warning("Failed to render TrueType font from UTF-8 character '{0}'!", utfChar);
            return false;
        }

        SDL_Rect src = {0, 0, texture->GetWidth(), texture->GetHeight()};
        importedData.push_back(
            (ImportedTextureData) {
                fontName + string("_") + Utilities::ToString(ptSize) + "_" + Utilities::ToString(style) + "_" + utfChar,
                src,
                false,
                texture
            }
        );

        return true;
    }

    int TexturePack::PackImported(Renderer& renderer, Uint32 pixelFormatting, bool smallestFirst, Uint16 maxSize)
    {
        // Free current pack texture and it's meta data
        FreePack();

        // Sort the imported textures. Small textures are first, big textures last
        // as small textures are likely to be reused more than large textures
        if (importedData.empty() || pixelFormatting == SDL_PIXELFORMAT_UNKNOWN)
        {
            return 0;
        }
        /*vector<ImportedTextureData> insertList;
        insertList.push_back(importedData[0]);
        // Inefficient but simple insertion sort. This should be optimised at some point,
        // but this method shouldn't be used during game runtime anyway
        for (int i = 1, counti = importedData.size(); i < counti; i++)
        {
            bool inserted = false;
            for (vector<ImportedTextureData>::iterator j = insertList.begin(); j != insertList.end(); j++)
            {
                if (smallestFirst ? compareImportedSmallestFirst(importedData[i], *j) : compareImportedBiggestFirst(importedData[i], *j))
                {
                    insertList.insert(j, importedData[i]);
                    inserted = true;
                    break;
                }
            }
            if (!inserted)
            {
                insertList.push_back(importedData[i]);
            }
        }*/
        // Once all the textures are sorted, we can start rendering them to the packedTexture
        int numAdded = 0;
        SDL_Renderer* render = renderer.GetRendererSDL();

        // Max size is the maximum area for packing the textures, but any leftover space will be removed.
        packedTexture.texture = SDL_CreateTexture(render, pixelFormatting, SDL_TEXTUREACCESS_TARGET, maxSize, maxSize);
        packedTexture.width = maxSize;
        packedTexture.height = maxSize;
        SDL_Texture* originalTarget = SDL_GetRenderTarget(render);
        SDL_SetRenderTarget(render, packedTexture.texture);

        // Marker rect is used to mark height and width of the current 'row' of textures
        //SDL_Rect markerRect = {0, 0, 0, 0};
        SDL_RenderClear(render);
        int finalWidth = 0;
        int finalHeight = 0;
        packData.clear();

        int totalRects = importedData.size();

        stbrp_context rect_packer;
        stbrp_rect* rects = new stbrp_rect[totalRects];

        for (int i = 0; i < totalRects; i++)
        {
            /*int originX = markerRect.x;
            int originY = markerRect.y;
            // Find a suitable spot
            if (markerRect.x > maxSize)
            {
                markerRect.x = 0;
                markerRect.y += markerRect.h;
                markerRect.h = 0;
            }*/
            // Setup the rect for packing and add it to the array
            stbrp_rect src;
            src.w = importedData[i].texture->GetWidth();
            src.h = importedData[i].texture->GetHeight();
            src.id = i;
            rects[i] = src;

            //SDL_Rect src = {0, 0, insertList[i].texture->GetWidth(), };
            //SDL_Rect dest = {markerRect.x, markerRect.y, src.w, src.h};
            /*markerRect.x += src.w;
            markerRect.h = src.h > markerRect.h ? src.h : markerRect.h;
            if (markerRect.y + markerRect.h > maxSize)
            {
                break;
            }
            SDL_RenderCopy(render, insertList[i].texture->texture, &src, &dest);
            finalWidth = finalWidth < markerRect.x ? markerRect.x : finalWidth;
            finalHeight = finalHeight < markerRect.y + markerRect.h ? markerRect.y + markerRect.h : finalHeight;
            insertList[i].pureClip.x = originX;
            insertList[i].pureClip.y = originY;
            packData.push_back((TextureData){insertList[i].path, insertList[i].pureClip, insertList[i].mipmapped});
            numAdded++;
            */
        }

        stbrp_node* nodes = new stbrp_node[maxSize + 1];
        stbrp_init_target(&rect_packer, maxSize, maxSize, nodes, maxSize + 1);

        if (!stbrp_pack_rects(&rect_packer, rects, totalRects))
        {
            // Not all rects were packed.
            Logger::EngineLog().Warning("TexturePack failed to pack some textures!");
        }

        // Now all the packed rects have been computed, render the corresponding textures to a blank texture.
        for (int i = 0; i < totalRects; i++)
        {
            if (rects[i].was_packed)
            {
                // Get destination and clip rects
                SDL_Rect dest = {rects[i].x, rects[i].y, rects[i].w, rects[i].h};
                SDL_Rect src = {0, 0, importedData[i].texture->GetWidth(), importedData[i].texture->GetHeight()};
                SDL_Rect metaClip = {dest.x, dest.y, importedData[i].pureClip.w, importedData[i].pureClip.h};

                // Render to texture
                importedData[i].texture->Render(render, dest, &src);

                // Copy meta data
                TextureData meta;
                meta.path = importedData[i].path;
                meta.pureClip = metaClip;
                meta.mipmapped = importedData[i].mipmapped;
                packData.push_back(meta);

                // Compute total rect area used
                finalWidth = max(dest.x + dest.w, finalWidth);
                finalHeight = max(dest.y + dest.h, finalHeight);

                numAdded++;
            }
            else
            {
                // Log warning
                Logger::EngineLog().Warning("TexturePack failed to pack texture '{0}'.", importedData[i].path);
            }
        }

        // Free memory used to pack the rects
        delete rects;
        rects = nullptr;
        delete nodes;
        nodes = nullptr;

        // Actually render onto the main texture
        SDL_RenderPresent(render);

        // Now remove all unused texture space
        SDL_Rect targetRect = {0, 0, finalWidth, finalHeight};
        SDL_Surface* renderSurface = SDL_CreateRGBSurface(0, targetRect.w, targetRect.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_RenderReadPixels(render, &targetRect, SDL_PIXELFORMAT_ARGB8888, renderSurface->pixels, renderSurface->pitch);
        packedTexture.Free();
        packedTexture.texture = SDL_CreateTextureFromSurface(render, renderSurface);
        packedTexture.width = renderSurface->w;
        packedTexture.height = renderSurface->h;
        SDL_FreeSurface(renderSurface);

        // Go back to rendering wherever we were rendering before
        SDL_SetRenderTarget(render, originalTarget);

        // Free imported textures now we've packed them all
        FreeImported();

        return numAdded;
    }

    bool TexturePack::Save(Renderer& renderer, Uint32 pixelFormatting, string path)
    {
        // Saves the texture pack as a PNG image with meta file containing clip information
        if (packedTexture.texture != NULL)
        {
            // Setup target texture and surface
            SDL_Renderer* render = renderer.GetRendererSDL();
            SDL_Texture* originalTarget = SDL_GetRenderTarget(render);
            SDL_Rect targetRect = {0, 0, packedTexture.width, packedTexture.height};
            SDL_Surface* renderSurface = SDL_CreateRGBSurface(0, targetRect.w, targetRect.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            if (renderSurface == NULL)
            {
                Logger::EngineLog().Error("Failed to create surface during TexturePack save process!");
                return false;
            }
            SDL_Texture* renderTarget = SDL_CreateTexture(render, pixelFormatting, SDL_TEXTUREACCESS_TARGET, targetRect.w, targetRect.h);
            if (renderTarget == NULL)
            {
                Logger::EngineLog().Error("Failed to create render target texture during TexturePack save process!");
                return false;
            }
            SDL_SetRenderTarget(render, renderTarget);

            // Render to texture and read the pixels to an SDL_Surface
            SDL_RenderClear(render);
            SDL_RenderCopy(render, packedTexture.texture, &targetRect, &targetRect);
            SDL_RenderPresent(render);
            SDL_RenderReadPixels(render, &targetRect, SDL_PIXELFORMAT_ARGB8888, renderSurface->pixels, renderSurface->pitch);
            // Free the original texture and reset the render target
            SDL_DestroyTexture(renderTarget);
            renderTarget = NULL;
            SDL_SetRenderTarget(render, originalTarget);

            // Save the image
            IMG_SavePNG(renderSurface, (path + ".png").c_str());

            // Free the surface
            SDL_FreeSurface(renderSurface);
            renderSurface = NULL;

            // Now save the meta data
            JSON data;
            SerialiseOut(data);
            data.Export(path + ".tpm");

            Logger::EngineLog().Info("Saved TexturePack at '{0}'.", path);
            return true;
        }
        else
        {
            Logger::EngineLog().Warning("Could not save TexturePack, source texture is NULL.");
        }
        return false;
    }

    Image& TexturePack::GetPackedTexture()
    {
        return packedTexture;
    }

    SDL_Rect TexturePack::GetClip(string textureId)
    {
        for (unsigned int i = 0, counti = packData.empty() ? 0 : packData.size(); i < counti; i++)
        {
            if (packData[i].path == textureId)
            {
                return packData[i].pureClip;
            }
        }
        Logger::EngineLog().Warning("Could not locate texture '{0}' in texture pack!", textureId);
        return {0, 0, 1, 1};
    }

    SDL_Rect TexturePack::GetClip(string textureId, int mipmapLevel)
    {
        return GetMipMapClip(GetClip(textureId), mipmapLevel);
    }

    bool TexturePack::compareImportedSmallestFirst(ImportedTextureData& i, ImportedTextureData& j)
    {
        return ((i.pureClip.h * i.pureClip.h) + (i.pureClip.w * i.pureClip.h)) < ((j.pureClip.h * j.pureClip.h) + (j.pureClip.w * j.pureClip.h));
    }

    bool TexturePack::compareImportedBiggestFirst(ImportedTextureData& i, ImportedTextureData& j)
    {
        return ((i.pureClip.h * i.pureClip.h) + (i.pureClip.w * i.pureClip.h)) > ((j.pureClip.h * j.pureClip.h) + (j.pureClip.w * j.pureClip.h));
    }

}

