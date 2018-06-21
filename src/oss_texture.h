#ifndef OSS_TEXTURE_H
#define OSS_TEXTURE_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

/// Wrapper class for SDL_Texture
class OSS_Texture
{
public:
    /// Appropriate constructor and destructor
    OSS_Texture();
    ~OSS_Texture();

    /// Destroys the texture and makes it NULL
    void freeTexture();

    /// Load an image file and returns true if it was successful, need window pixel format for pixel manipulation
    bool loadImage(string path, SDL_Renderer* renderer, Uint32 windowPixelFormat = SDL_PIXELFORMAT_UNKNOWN);

    /// Creates a texture using a TrueType Font and a given string, returns success
    bool createText(string text, SDL_Renderer* renderer, TTF_Font* font, SDL_Color color);

    /// Set mod blending mode
    void setBlendMode(SDL_BlendMode blend);

    /// Set alpha and color modulation respectively
    void setAlpha(Uint8 a);
    void setColorMod(Uint8 r, Uint8 g, Uint8 b);

    /// Renders the texture to video memory; supports simple texture clipping
    void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL, float angle = 0.0, SDL_Point* origin = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    /// Get and set dimensions
    int getWidth();
    int getHeight();
    void setWidth(int w);
    void setHeight(int h);

    /// In order to manipulate the individual pixels of a texture, we need to lock/unlock it as necessary
    bool lockPixels();
    bool unlockPixels();

    /// Get the individual pixels from texture
    void* getPixels();
    /// Get the pixel 'width' of the texture in memory
    int getPitch();

private:
    /// Actual texture image
    SDL_Texture* texture;

    /// Texture dimensions
    int width;
    int height;

    /// Pixels data
    void* pixels;
    int pitch;

};

#endif // OSS_TEXTURE_H
