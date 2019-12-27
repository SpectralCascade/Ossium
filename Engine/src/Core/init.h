/** COPYRIGHT NOTICE
 *  
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
#ifndef INIT_H
#define INIT_H

#include "helpermacros.h"

namespace Ossium
{
    /// Initialisation error types
    enum InitError
    {
        INIT_ERROR_SDL = -9999,
        INIT_ERROR_IMG,
        INIT_ERROR_MIXER,
        INIT_ERROR_TTF
    };

    /// Initialises SDL and other engine subsystems; returns InitError value. No error if >= 0
    OSSIUM_EDL int InitialiseOssium();

    /// Terminates SDL and other engine subsystems
    OSSIUM_EDL void TerminateOssium();

}

#endif // INIT_H
