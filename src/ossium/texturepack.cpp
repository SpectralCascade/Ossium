#include <string.h>
#include <vector>
#include <SDL2/SDL.h>

#include "basics.h"
#include "renderer.h"
#include "texture.h"
#include "texturepack.h"

using namespace std;

namespace ossium
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
        freeAll();
    }

    void TexturePack::freeAll()
    {
        freePack();
        freeImported();
    }

    void TexturePack::freePack()
    {
        packData.clear();
        packedTexture.freeTexture();
    }

    void TexturePack::freeImported()
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

    bool TexturePack::load(string path, int* args)
    {
        freePack();
        return packedTexture.load(path, args);
    }

    bool TexturePack::init(Renderer* renderer, Uint32 windowPixelFormat)
    {
        return packedTexture.init(renderer, windowPixelFormat);
    }

    bool TexturePack::import(string path, Renderer* renderer, Uint32 windowPixelFormat, int minMipMapSize)
    {
        Texture* importedTexture = new Texture();
        if (!importedTexture->load(path, NULL) || !importedTexture->init(renderer, windowPixelFormat))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to import texture into texture pack with path '%s'", path.c_str());
            return false;
        }
        bool mipmapped = false;
        SDL_Rect src = {0, 0, importedTexture->getWidth(), importedTexture->getHeight()};
        // If a pixel format is provided, we will try and generate a mipmap for the imported texture
        if (windowPixelFormat != SDL_PIXELFORMAT_UNKNOWN)
        {
            SDL_Texture* mipmappedTexture = NULL;
            SDL_Renderer* targetRenderer = renderer->getRenderer();
            mipmappedTexture = SDL_CreateTexture(targetRenderer, windowPixelFormat, SDL_TEXTUREACCESS_TARGET, (src.w / 2) * 3, src.h);
            // Cache original renderer target
            SDL_Texture* originalTarget = SDL_GetRenderTarget(targetRenderer);
            // Configure the renderer so it renders to the mipmapped texture
            SDL_SetRenderTarget(targetRenderer, mipmappedTexture);
            SDL_RenderClear(targetRenderer);
            // Render the imported texture to the mipmapped texture
            SDL_RenderCopy(targetRenderer, importedTexture->texture, NULL, &src);
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
                SDL_RenderCopy(targetRenderer, importedTexture->texture, &src, &mipmapClip);
                level++;
                previousClip = mipmapClip;
            }
            SDL_RenderPresent(targetRenderer);
            SDL_Rect targetRect = {0, 0, (src.w / 2) * 3, src.h};
            SDL_Surface* renderSurface = SDL_CreateRGBSurface(0, targetRect.w, targetRect.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            SDL_RenderReadPixels(targetRenderer, &targetRect, SDL_PIXELFORMAT_ARGB8888, renderSurface->pixels, renderSurface->pitch);
            mipmappedTexture = SDL_CreateTextureFromSurface(targetRenderer, renderSurface);
            // Free the original imported texture and replace with mipmapped version
            SDL_DestroyTexture(importedTexture->texture);
            importedTexture->texture = mipmappedTexture;
            importedTexture->width = renderSurface->w;
            importedTexture->height = renderSurface->h;

            SDL_FreeSurface(renderSurface);
            // Reset the render target of the renderer
            SDL_SetRenderTarget(targetRenderer, originalTarget);
            mipmapped = true;
        }
        importedData.push_back((ImportedTextureData){path, src, mipmapped, importedTexture});
        return true;
    }

    int TexturePack::packImported(Renderer* renderer, Uint32 windowPixelFormat, bool smallestFirst, Uint16 maxSize)
    {
        // Free current pack texture and it's meta data
        freePack();
        // Sort the imported textures. Small textures are first, big textures last
        // as small textures are likely to be reused more than large textures
        if (importedData.empty() || windowPixelFormat == SDL_PIXELFORMAT_UNKNOWN)
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
        SDL_Renderer* render = renderer->getRenderer();
        // Max size is used for dimensions for time being, but any leftover space will be removed
        packedTexture.texture = SDL_CreateTexture(render, windowPixelFormat, SDL_TEXTUREACCESS_TARGET, maxSize, maxSize);
        packedTexture.width = maxSize;
        packedTexture.height = maxSize;
        SDL_Texture* originalTarget = SDL_GetRenderTarget(render);
        SDL_SetRenderTarget(render, packedTexture.texture);
        // Marker rect is used to mark height and width of the current 'row' of textures
        SDL_Rect markerRect = {0, 0, maxSize, maxSize};
        SDL_RenderClear(render);
        int finalWidth = 0;
        int finalHeight = 0;
        for (int i = 0, counti = insertList.size(); i < counti; i++)
        {
            // Find a suitable spot
            if (markerRect.x > maxSize)
            {
                markerRect.x = 0;
                markerRect.y += markerRect.h;
                markerRect.h = 0;
            }
            SDL_Rect src = {0, 0, insertList[i].texture->getWidth(), insertList[i].texture->getHeight()};
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
            numAdded++;
        }
        SDL_RenderPresent(render);

        SDL_Rect targetRect = {0, 0, finalWidth, finalHeight};
        SDL_Surface* renderSurface = SDL_CreateRGBSurface(0, targetRect.w, targetRect.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_RenderReadPixels(render, &targetRect, SDL_PIXELFORMAT_ARGB8888, renderSurface->pixels, renderSurface->pitch);
        packedTexture.texture = SDL_CreateTextureFromSurface(render, renderSurface);
        packedTexture.width = renderSurface->w;
        packedTexture.height = renderSurface->h;

        SDL_FreeSurface(renderSurface);
        SDL_SetRenderTarget(render, originalTarget);
        insertList.clear();
        freeImported();
        return numAdded;
    }

    // Todo: implement me
    bool TexturePack::save(string path)
    {
        return false;
    }

    Texture& TexturePack::getPackedTexture()
    {
        return packedTexture;
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
