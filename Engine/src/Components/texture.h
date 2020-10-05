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

namespace Ossium
{

    enum TextureLayoutMode
    {
        NONE = 0,
        FIT_X,
        FIT_Y,
        FIT_XY,
        FIT_ASPECT
    };

    struct OSSIUM_EDL TextureSchema : public MetaRect
    {
    public:
        DECLARE_SCHEMA(TextureSchema, MetaRect);

        /// Path to the image file this texture should display
        SCHEMA_MEMBER(ATTRIBUTE_FILEPATH, std::string, imgPath);

        /// The area of the source image that should be rendered
        M(SDL_Rect, clip) = {0, 0, 0, 0};

        /// Should this texture be tiled?
        M(bool, tiled) = false;

        /// Should this texture be flipped vertically, horizontally, or not at all?
        M(SDL_RendererFlip, flip) = SDL_FLIP_NONE;

        /// Colour and alpha modulation values. These are applied whenever the Render() method is called
        M(SDL_Color, modulation) = {0xFF, 0xFF, 0xFF, 0xFF};

        /// The blending mode for this texture. Applied whenever the Render() method is called
        M(SDL_BlendMode, blending) = SDL_BLENDMODE_BLEND;

        // Whether the texture should stretch x, y, x and y, aspect-fit or ignore a BoxLayout. 
        // By default the image simply stretches to fit x and y.
        M(TextureLayoutMode, fitLayout) = FIT_XY;

        // 9-slicing support.
        // Defines vertical and horizontal slices which can be stretched, the rest of the source image
        // is never stretched.
        M(SDL_Rect, stretchArea) = {0, 0, 0, 0};

    };

    /// This class is used for rendering an image
    class OSSIUM_EDL Texture : public GraphicComponent, public TextureSchema
    {
    public:
        DECLARE_COMPONENT(GraphicComponent, Texture);
        CONSTRUCT_SCHEMA(GraphicComponent, TextureSchema);

        virtual ~Texture(){};

        /// Once loaded, find the source image.
        void OnLoadFinish();

        /// Sets the alpha blending mode
        void SetBlendMode(SDL_BlendMode blend);

        /// Sets alpha modulation
        void SetAlphaMod(Uint8 a);
        /// Sets color modulation
        void SetColorMod(Uint8 r, Uint8 g, Uint8 b);
        /// Sets both color and alpha modulation
        void SetMod(SDL_Color mod);

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
