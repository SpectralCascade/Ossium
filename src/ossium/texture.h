#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

namespace ossium
{
    /// Wrapper class for SDL_Texture
    class Texture
    {
    public:
        /// Appropriate constructor and destructor
        Texture();
        ~Texture();

        /// Destroys the texture and makes it NULL
        void freeTexture();

        /// Load an image file to a surface and returns true if it was successful
        bool load(string guid_path, int* loadArgs = NULL);

        /// Post-load texture initialisation; pass the window pixel format if you wish to manipulate
        /// the texture's pixel data
        bool init(SDL_Renderer* renderer, Uint32 windowPixelFormat = SDL_PIXELFORMAT_UNKNOWN);

        #ifdef _SDL_TTF_H
        /// Creates a texture using a TrueType Font and a given string, returns success
        /// TODO: Transfer and refactor this method to a new class dedicated to text rendering
        bool createText(string text, SDL_Renderer* renderer, TTF_Font* font, SDL_Color color);
        #endif // _SDL_TTF_H

        /// Set mod blending mode
        void setBlendMode(SDL_BlendMode blend);

        /// Set alpha and color modulation respectively
        void setAlpha(Uint8 a);
        void setColorMod(Uint8 r, Uint8 g, Uint8 b);

        /// Renders the texture to video memory; supports simple texture clipping
        void render(SDL_Renderer* renderer, SDL_Rect dest, SDL_Rect* clip = NULL, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /// Simplified overload method
        void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        /// Even simpler render method
        void renderSimple(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL);

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
