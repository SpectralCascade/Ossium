#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include "../Core/coremaths.h"
#include "../Core/component.h"
#include "../Core/colors.h"
#include "../Core/metarect.h"
#include "../Core/font.h"
#include "../Core/resourcecontroller.h"
#include "../Core/image.h"

using namespace std;

namespace Ossium
{

    struct TextureSchema : public MetaRect
    {
    public:
        DECLARE_SCHEMA(TextureSchema, Schema<MetaRectSchema>);

        /// Path to the image file this texture should display
        M(string, imgPath);

        /// The area of the source image that should be rendered
        M(SDL_Rect, clip) = {0, 0, 0, 0};

        /// Should this texture be flipped vertically, horizontally, or not at all?
        M(SDL_RendererFlip, flip) = SDL_FLIP_NONE;

        /// Colour and alpha modulation values. These are applied whenever the Render() method is called
        M(SDL_Color, modulation) = {0xFF, 0xFF, 0xFF, 0xFF};

        /// The blending mode for this texture. Applied whenever the Render() method is called
        M(SDL_BlendMode, blending) = SDL_BLENDMODE_BLEND;

    };

    /// This class is used for rendering an image
    class OSSIUM_EDL Texture : public GraphicComponent, public TextureSchema
    {
    public:
        DECLARE_COMPONENT(Texture);
        CONSTRUCT_SCHEMA(GraphicComponent, TextureSchema);

        virtual ~Texture(){};

        /// Updates any derived instance if necessary.
        void Update();

        /// Once loaded, find the source image.
        void OnLoadFinish();

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

    };

}

#endif // TEXTURE_H
