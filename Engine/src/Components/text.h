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

#include "../Core/component.h"
#include "../Core/helpermacros.h"
#include "../Core/textlayout.h"

namespace Ossium
{

    struct TextSchema : public Schema<TextSchema, 20>
    {
        DECLARE_BASE_SCHEMA(TextSchema, 20);

        /// The text string
        M(std::string, text) = "";

        M(bool, applyMarkup) = true;

        /// GUID of font to use:
        SCHEMA_MEMBER(ATTRIBUTE_FILEPATH, std::string, font_guid) = "";

        M(TextLayout, layout);

        /// If true, render box behind text in the background color
        M(bool, boxed) = false;

        /// Box padding, in pixels
        M(int, boxPaddingWidth) = 4;
        M(int, boxPaddingHeight) = 2;

        /// Background colour for outlines or shaded rendering box
        M(SDL_Color, backgroundColor) = {0x00, 0x00, 0x00, 0xFF};

        /// Text outline thickness in pixels
        M(int, outline) = 0;

        /// Font hinting, e.g. monospace
        M(int, hinting) = TTF_HINTING_NORMAL;

        /// Solid, shaded or blended (alpha)
        M(int, renderMode) = RENDERTEXT_SOLID;

    };

    class OSSIUM_EDL Text : public GraphicComponent, public TextSchema
    {
    public:
        CONSTRUCT_SCHEMA(GraphicComponent, TextSchema);
        DECLARE_COMPONENT(GraphicComponent, Text);

        void OnLoadFinish();

        /// Graphic override
        void Render(Renderer& renderer);

        bool dirty = true;

    private:
        /// Pointer to font
        Font* font = nullptr;

    };

}

#endif // TEXT_H
