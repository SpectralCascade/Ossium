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
#ifndef CONFIG_H
#define CONFIG_H

#include "schemamodel.h"
#include "stringconvert.h"

namespace Ossium
{

    /// General configuration information for Ossium
    struct OSSIUM_EDL Config : public Schema<Config, 20>
    {
        DECLARE_BASE_SCHEMA(Config, 20);

        M(std::string, windowTitle) = "Ossium Engine";
        M(int, windowWidth) = 640;
        M(int, windowHeight) = 480;
        M(int, windowFlags) = SDL_WINDOW_SHOWN;
        M(bool, fullscreen) = false;
        M(bool, vsync) = true;
        M(bool, hardwareAcceleration) = true;
        M(int, totalRenderLayers) = 100;
        M(float, fpscap) = 0;
        M(char, filtering) = '1';
        M(unsigned int, mastervolume) = 100;
        M(std::vector<std::string>, startScenes) = {};

    };

}

#endif // CONFIG_H
