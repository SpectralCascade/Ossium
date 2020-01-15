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
#include "font.h"
#include "logging.h"

using namespace std;

namespace Ossium
{

    REGISTER_RESOURCE(Font);

    Font::Font()
    {
        font = NULL;
    }

    Font::~Font()
    {
        Free();
    }

    void Font::Free()
    {
        font = NULL;
        if (!fontBank.empty())
        {
            if (TTF_WasInit() > 0)
            {
                for (auto i = fontBank.begin(); i != fontBank.end(); i++)
                {
                    TTF_Font* tempFont = i->second;
                    if (tempFont != NULL)
                    {
                        TTF_CloseFont(tempFont);
                        tempFont = NULL;
                    }
                }
            }
            fontBank.clear();
        }
    }

    bool Font::Load(string guid_path, vector<int> pointSizes)
    {
        Free();
        path = guid_path;
        if (pointSizes.empty())
        {
            /// Load up a single default font size
            font = TTF_OpenFont(guid_path.c_str(), 24);
            if (font != NULL)
            {
                fontBank[24] = font;
            }
        }
        else
        {
            TTF_Font* tempFont = NULL;
            for (int ptsize : pointSizes)
            {
                tempFont = TTF_OpenFont(guid_path.c_str(), ptsize);
                if (tempFont == NULL)
                {
                    Logger::EngineLog().Error("Failed to open font '{0}' with point size '{1}'! TTF_Error: {2}", guid_path, ptsize, TTF_GetError());
                    continue;
                }
                if (font == NULL)
                {
                    /// Default to first provided point size
                    font = tempFont;
                }
                fontBank[ptsize] = tempFont;
            }
        }
        if (font == NULL)
        {
            Logger::EngineLog().Error("Failed to open font '{0}'! TTF_Error: {1}", guid_path, TTF_GetError());
        }
        return font != NULL;
    }

    bool Font::LoadAndInit(string guid_path, vector<int> pointSizes)
    {
        return Load(guid_path, pointSizes) && Init(guid_path);
    }

    bool Font::Init(string guid_path)
    {
        return true;
    }

    SDL_Rect Font::GetGlyphClipRect(Renderer& renderer, string utfChar, int pointSize, int style)
    {
    }

    TTF_Font* Font::GetFont(int pointSize)
    {
        if (pointSize > 0)
        {
            TTF_Font* temp = NULL;
            auto i = fontBank.find(pointSize);
            if (i != fontBank.end())
            {
                temp = i->second;
            }
            if (temp != NULL)
            {
                font = temp;
            }
            else
            {
                // Dynamically attempt to load the font at the specified point size
                temp = TTF_OpenFont(path, pointSize);
                if (temp == NULL)
                {
                    Logger::EngineLog().Error("Failed to open font '{0}' at point size {1}! TTF_Error: {2}", guid_path, pointSize, TTF_GetError());
                }
                else
                {
                    fontBank[pointSize] = temp;
                    font = temp;
                }
            }
        }
        return font;
    }

}
