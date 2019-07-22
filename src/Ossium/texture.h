#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#include "coremaths.h"
#include "ecs.h"
#include "colors.h"
#include "primitives.h"
#include "metarect.h"
#include "font.h"
#include "resourcecontroller.h"

using namespace std;

namespace Ossium
{

    inline namespace Structs
    {
        struct Rect;
        struct Point;
    }

    enum TextRenderModes
    {
        RENDERTEXT_SOLID = 0,
        RENDERTEXT_SHADED,
        RENDERTEXT_BLEND
    };

    /// Forward declaration
    class TexturePack;

    inline namespace Graphics
    {

        /// Forward declare Texture
        class Texture;

        /// This class wraps an SDL_Texture resource
        class Image
        {
        public:
            Image() = default;
            Image(const Image& source);
            ~Image();

            friend class Texture;
            friend class Ossium::TexturePack;

            /// Destroys the image, freeing it from memory. Does not modify the temporary SDL_Surface
            void Free();

            /// Load an image and returns true if it was successful
            bool Load(string guid_path);

            /// Creates an image with a text string
            bool CreateFromText(Renderer& renderer, Font& fontToUse, string text, int pointSize = 12, SDL_Color color = Colors::RED,
                                int hinting = 0, int kerning = 0, int outline = 0, int style = 0, int renderMode = 0, SDL_Color bgColor = Colors::BLACK);

            /// Post-load texture initialisation; pass the window pixel format if you wish to manipulate pixel data.
            /// You MUST call this method after successfully calling Load() if you wish to render the image to the screen.
            /// If cache is set to true, the original image is stored in memory as an SDL_Surface for quick initialisation without loading the image again
            /// (in addition to the modifiable SDL_Texture in video memory)! This could be useful if you want to remove all applied effects on the image frequently,
            /// as long as your target system has adequate memory capacity for storing the original image. Leave this set to false unless you know what you're doing!
            bool Init(Renderer& renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_ARGB8888, bool cache = false);

            /// Shorthand method
            bool LoadAndInit(string guid_path, Renderer& renderer, Uint32 pixelFormatting = SDL_PIXELFORMAT_ARGB8888, bool cache = false);

            /// Returns true if the texture is not NULL
            bool Initialised();

            /// Returns the width of the image
            int GetWidth();
            /// Returns the height of the image
            int GetHeight();

            /// Returns the name of the loaded image, if it has one.
            string GetPathName();

            /// Applies a pixel manipulation function to the image. The argument function accepts pixel data as an SDL_Color
            /// in addition to an SDL_Point which indicates which pixel is being modified relative to the top left of the image.
            /// It works by iterating over each pixel in the source image and giving your function relevant information. The function must return an SDL_Color value which is what the pixel
            /// will be set to. Note that the image must be initialised with the window's pixel format to apply pixel manipulation effects!
            /// Also note that this doesn't do it's work on the GPU, so be wary of using it frequently as it's pretty expensive
            /// Returns false on failure.
            template<class Func>
            bool ApplyEffect(Func f, SDL_Rect* clipArea = nullptr)
            {
                if (LockPixels())
                {
                    Uint32* pixelArray = reinterpret_cast<Uint32*>(pixels);
                    SDL_PixelFormat* pixelFormat = SDL_AllocFormat(format);
                    if (pixelFormat == NULL)
                    {
                        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not allocate pixel format to apply effect! SDL_Error: %s", SDL_GetError());
                        return false;
                    }
                    SDL_Color pixelData;
                    SDL_Point pixelPos;
                    if (clipArea == nullptr)
                    {
                        for (int i = 0, counti = (pitch / 4) * height; i < counti; i++)
                        {
                            pixelData = ConvertToColor(pixelArray[i], pixelFormat);
                            pixelPos.x = i % width;
                            pixelPos.y = i / width;
                            SDL_Color outputColor = f(pixelData, pixelPos);
                            pixelArray[i] = SDL_MapRGBA(pixelFormat, outputColor.r, outputColor.g, outputColor.b, outputColor.a);
                        }
                    }
                    else
                    {
                        int memwidth = (pitch / 4);
                        int clipskipy = (memwidth * clipArea->y);
                        int clipxw = clipArea->w + clipArea->x;
                        int increment = (memwidth - clipArea->w);
                        for (int i = clipskipy + clipArea->x, counti = memwidth * clipArea->h; i < counti; i++)
                        {
                            if (i >= clipxw + clipskipy)
                            {
                                i += increment;
                                clipskipy += memwidth;
                                if (i > counti)
                                {
                                    break;
                                }
                            }
                            pixelData = ConvertToColor(pixelArray[i], pixelFormat);
                            pixelPos.x = i % width;
                            pixelPos.y = i / width;
                            SDL_Color outputColor = f(pixelData, pixelPos);
                            pixelArray[i] = SDL_MapRGBA(pixelFormat, outputColor.r, outputColor.g, outputColor.b, outputColor.a);
                        }
                    }
                    UnlockPixels();
                    SDL_FreeFormat(pixelFormat);
                    return true;
                }
                return false;
            };

        protected:
            /// Locks the texture so the raw pixels may be modified
            bool LockPixels();
            /// Unlocks the texture so the raw pixels can no longer be modified
            bool UnlockPixels();

            /// The path used to load the current image.
            string pathname = "";

            /// The image prior to being converted to an SDL_Texture
            SDL_Surface* tempSurface = NULL;

            /// A representation of the image in video memory
            SDL_Texture* texture = NULL;

            /// Outline texture (used purely for rendered text)
            SDL_Texture* outlineTexture = NULL;

            /// Image dimensions
            int width = 0;
            int height = 0;

            /// The pixel format
            Uint32 format;
            /// Pixel data; NULL unless you have locked the pixels in video memory
            void* pixels = NULL;
            /// 'Width' of the image as laid out in memory, in bytes
            int pitch = 0;

        };

        /// This class is used for rendering an image
        class Texture : public GraphicComponent, public MetaRect
        {
        public:
            DECLARE_COMPONENT(Texture);

            virtual ~Texture(){};

            /// Updates any derived instance if necessary.
            void Update();

            /// Sets the alpha blending mode
            void SetBlendMode(SDL_BlendMode blend, bool immediate = false);

            /// Sets alpha modulation
            void SetAlphaMod(Uint8 a, bool immediate = false);
            /// Sets color modulation
            void SetColorMod(Uint8 r, Uint8 g, Uint8 b, bool immediate = false);
            /// Sets both color and alpha modulation
            void SetMod(SDL_Color mod, bool immediate = false);

            /// Returns the color and alpha modulation values for this texture
            SDL_Color GetMod();

            /// Inherited Graphic::Render() method
            void Render(Renderer& renderer);

            /// Sets the source image this texture should use. If configureDimensions is true, the width and height are set
            /// to the source image width and height
            void SetSource(Image* src, bool configureDimensions = true);
            /// Returns a pointer to the source image
            Image* GetSource();

            /// Sets the width as a percentage of the source image width
            virtual void SetRenderWidth(float percent);
            /// Sets the height as a percentage of the source image height
            virtual void SetRenderHeight(float percent);

            /// Sets the flip mode of the texture; a texture can be flipped horizontally, vertically, or not at all
            void SetFlip(SDL_RendererFlip flipMode);
            /// Sets the clip rect area. Set autoScale to false to prevent resizing the rendered dimensions
            virtual void SetClip(int x, int y, int w = 0, int h = 0, bool autoScale = true);

            /// Gets the source image width
            int GetSourceWidth();
            /// Gets the source image height
            int GetSourceHeight();
            /// Gets the width as a percentage of the clip width
            float GetRenderWidth();
            /// Ditto, but as a percentage of the clip height
            float GetRenderHeight();
            /// Gets the flip mode of the texture; a texture can be flipped horizontally, vertically, or not at all
            SDL_RendererFlip GetFlip();
            /// Returns the clip area of the image to be rendered
            SDL_Rect GetClip();

            /// Takes a point from UI space and transforms it to the local texture space
            Point ScreenToLocalPoint(Point source);

        protected:
            /// The source image that this texture renders a copy of
            Image* source = nullptr;

            /// The area of the source image that should be rendered
            SDL_Rect clip = {0, 0, 0, 0};

            /// Should this texture be flipped vertically, horizontally, or not at all?
            SDL_RendererFlip flip = SDL_FLIP_NONE;

            /// Colour and alpha modulation values. These are applied whenever the Render() method is called
            SDL_Color modulation = {0xFF, 0xFF, 0xFF, 0xFF};

            /// The blending mode for this texture. Applied whenever the Render() method is called
            SDL_BlendMode blending = SDL_BLENDMODE_BLEND;

        };

    }

}

#endif // TEXTURE_H
