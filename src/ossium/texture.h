#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

using namespace std;

namespace Ossium
{

    /// Forward declare Renderer and TexturePack
    inline namespace graphics
    {
        class Renderer;
    }
    class TexturePack;

    /// Wrapper class for SDL_Texture
    class Texture
    {
    public:
        /// Renderer has access to the actual rendering methods
        friend class graphics::Renderer;
        /// TexturePack is allowed to use the SDL_Texture
        friend class TexturePack;

        /// Appropriate constructor, copy constructors and destructor
        Texture();
        ~Texture();

        /// Destroys the texture and makes it NULL
        void freeTexture();

        /// Load an image file to a surface and returns true if it was successful
        bool load(string guid_path, int* loadArgs = NULL);

        /// Post-load texture initialisation; pass the window pixel format if you wish to manipulate
        /// the texture's pixel data
        bool init(Renderer* renderer, Uint32 windowPixelFormat = SDL_PIXELFORMAT_UNKNOWN);

        #ifdef _SDL_TTF_H
        /// Creates a texture using a TrueType Font and a given string, returns success
        /// This method provides a simple alternative to using the Text class for basic text rendering
        bool createText(string text, Renderer* renderer, TTF_Font* font, SDL_Color color);
        #endif // _SDL_TTF_H

        /// Set mod blending mode
        void setBlendMode(SDL_BlendMode blend);

        /// Set alpha and color modulation respectively
        void setAlpha(Uint8 a);
        void setColorMod(Uint8 r, Uint8 g, Uint8 b);

        /// Renders the texture to video memory; supports simple texture clipping
        void render(Renderer* renderer, SDL_Rect dest, SDL_Rect* clip = NULL, int layer = 0, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /// Simplified overload method
        void render(Renderer* renderer, int x, int y, SDL_Rect* clip = NULL, int layer = 0, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /// Even simpler render method
        void renderSimple(Renderer* renderer, int x, int y, SDL_Rect* clip = NULL, int layer = 0);
        void renderSimple(Renderer* renderer, SDL_Rect dest, SDL_Rect* clip = NULL, int layer = 0);

        /// Get texture dimensions
        int getWidth();
        int getHeight();

        /// In order to manipulate the individual pixels of a texture, we need to lock/unlock it as necessary
        bool lockPixels();
        bool unlockPixels();

        /// Get the individual pixels from texture
        void* getPixels();
        /// Get the pixel 'width' of the texture in memory
        int getPitch();

    private:
        /// Copying is prohibited as it requires SDL function calls with an SDL_Renderer
        Texture(const Texture& thisCopy);
        Texture operator=(const Texture& thisCopy);

        /// Renderer access only methods for actual rendering
        void renderTexture(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip = NULL, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
        void renderTexture(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
        void renderTextureSimple(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL);
        void renderTextureSimple(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip = NULL);

        /// Actual image as temporary SDL_Surface; converted to SDL_Texture by post-load initialisation method
        SDL_Surface* tempSurface;

        /// Texture image
        SDL_Texture* texture;

        /// Texture dimensions
        int width;
        int height;

        /// Pixels data
        void* pixels;
        int pitch;

    };

}

#endif // TEXTURE_H
