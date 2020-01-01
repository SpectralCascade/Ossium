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
#include <string.h>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

#include "funcutils.h"
#include "renderer.h"
#include "texture.h"
#include "texturepack.h"
#include "serialisation.h"

using namespace std;

namespace Ossium
{

    SDL_Rect getMipMapClip(SDL_Rect src, int level, Uint16 minSize)
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

    TexturePack::TexturePack()
    {
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

    bool TexturePack::Load(string path, int* args)
    {
        FreePack();
        // Load meta data first
        BinarySerialiser metaStream;
        metaStream.OpenSector("Ossium TexturePack Meta Data", path + ".tpm", READ);
        int packSize = 0;
        metaStream.Read(packSize);
        for (int i = 0; i < packSize; i++)
        {
            packData.push_back((TextureData){"", (SDL_Rect){0, 0, 0, 0}, false});
            metaStream.ReadString(packData[i].path);
            metaStream.Read(packData[i].mipmapped);
            metaStream.Read(packData[i].pureClip.x);
            metaStream.Read(packData[i].pureClip.y);
            metaStream.Read(packData[i].pureClip.w);
            metaStream.Read(packData[i].pureClip.h);
        }
        metaStream.CloseSector(READ);
        metaStream.Close(READ);
        // Now load the texture itself
        return packedTexture.Load(path + ".png");
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
        // If a pixel format is provided, we will try and generate a mipmap for the imported texture
        if (pixelFormatting != SDL_PIXELFORMAT_UNKNOWN)
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
                SDL_Rect mipmapClip = getMipMapClip(src, level, minMipMapSize);
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
        importedData.push_back((ImportedTextureData){path, src, mipmapped, importedTexture});
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
        vector<ImportedTextureData> insertList;
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
        }
        // Once all the textures are sorted, we can start rendering them to the packedTexture
        int numAdded = 0;
        SDL_Renderer* render = renderer.GetRendererSDL();
        // Max size is used for dimensions for time being, but any leftover space will be removed
        packedTexture.texture = SDL_CreateTexture(render, pixelFormatting, SDL_TEXTUREACCESS_TARGET, maxSize, maxSize);
        packedTexture.width = maxSize;
        packedTexture.height = maxSize;
        SDL_Texture* originalTarget = SDL_GetRenderTarget(render);
        SDL_SetRenderTarget(render, packedTexture.texture);
        // Marker rect is used to mark height and width of the current 'row' of textures
        SDL_Rect markerRect = {0, 0, 0, 0};
        SDL_RenderClear(render);
        int finalWidth = 0;
        int finalHeight = 0;
        packData.clear();
        for (int i = 0, counti = insertList.size(); i < counti; i++)
        {
            int originX = markerRect.x;
            int originY = markerRect.y;
            // Find a suitable spot
            if (markerRect.x > maxSize)
            {
                markerRect.x = 0;
                markerRect.y += markerRect.h;
                markerRect.h = 0;
            }
            SDL_Rect src = {0, 0, insertList[i].texture->GetWidth(), insertList[i].texture->GetHeight()};
            SDL_Rect dest = {markerRect.x, markerRect.y, src.w, src.h};
            markerRect.x += src.w;
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
        }
        SDL_RenderPresent(render);

        SDL_Rect targetRect = {0, 0, finalWidth, finalHeight};
        SDL_Surface* renderSurface = SDL_CreateRGBSurface(0, targetRect.w, targetRect.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_RenderReadPixels(render, &targetRect, SDL_PIXELFORMAT_ARGB8888, renderSurface->pixels, renderSurface->pitch);
        packedTexture.Free();
        packedTexture.texture = SDL_CreateTextureFromSurface(render, renderSurface);
        packedTexture.width = renderSurface->w;
        packedTexture.height = renderSurface->h;

        SDL_FreeSurface(renderSurface);
        SDL_SetRenderTarget(render, originalTarget);
        insertList.clear();
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

            // Actually save the image
            IMG_SavePNG(renderSurface, (path + ".png").c_str());

            // Free the surface
            SDL_FreeSurface(renderSurface);
            renderSurface = NULL;

            // Now save the meta data
            BinarySerialiser metaStream;
            metaStream.OpenSector("Ossium TexturePack Meta Data", path + ".tpm", WRITE);
            int packSize = packData.empty() ? 0 : (int)packData.size();
            metaStream.Write(packSize);
            for (int i = 0; i < packSize; i++)
            {
                metaStream.WriteString(packData[i].path);
                metaStream.Write(packData[i].mipmapped);
                metaStream.Write(packData[i].pureClip.x);
                metaStream.Write(packData[i].pureClip.y);
                metaStream.Write(packData[i].pureClip.w);
                metaStream.Write(packData[i].pureClip.h);
            }
            metaStream.CloseSector(WRITE);
            metaStream.Close(WRITE);
            packSize = 0;

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
        return getMipMapClip(GetClip(textureId), mipmapLevel);
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

