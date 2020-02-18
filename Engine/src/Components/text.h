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
#ifndef TEXT_H
#define TEXT_H

#include <string>

#include "../Core/font.h"
#include "../Core/component.h"
#include "../Core/helpermacros.h"

using namespace std;

namespace Ossium
{

    class OSSIUM_EDL Text : public GraphicComponent
    {
    public:
        DECLARE_COMPONENT(GraphicComponent, Text);

        /// Graphic override
        void Render(Renderer& renderer);

        /// Returns the dimensions of the text, or the dimensions of the box
        /// if the box is enabled
        /// TODO
        //int GetWidth();
        //int GetHeight();

        /// If true, render box behind text in the background color
        bool boxed = false;

        /// Box padding, in pixels
        int boxPaddingWidth = 4;
        int boxPaddingHeight = 2;

        /// The text string
        string text = "";

        /// Text colour
        SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};

        /// Background colour for outlines or shaded rendering box
        SDL_Color backgroundColor = {0x00, 0x00, 0x00, 0xFF};

        /// Text outline thickness in pixels
        int outline = 0;

        /// General text style, e.g. bold, italics etc.
        int style = TTF_STYLE_NORMAL;

        /// Whether or not kerning should be enabled
        bool kerning = true;

        /// Font hinting, e.g. monospace
        int hinting = TTF_HINTING_NORMAL;

        /// Solid, shaded or blended (alpha)
        int renderMode = RENDERTEXT_SOLID;

    private:
        /// Pointer to font
        Font* font = nullptr;

        /// GUID of font to use:
        string font_guid;

    };

}

#endif // TEXT_H
