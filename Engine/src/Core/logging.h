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
#ifndef LOGGING_H
#define LOGGING_H

#include "stringconvert.h"

namespace Ossium
{

    class OSSIUM_EDL Logger
    {
    public:
        template<typename ...Args>
        void Info(std::string message, Args&&... args)
        {
            message = Format(message, std::forward<Args>(args)...);
            SDL_LogInfo(SDL_LOG_CATEGORY_CUSTOM, "%s", message.c_str());
        }

        template<typename ...Args>
        void Warning(std::string message, Args&&... args)
        {
            message = Format(message, std::forward<Args>(args)...);
            SDL_LogWarn(SDL_LOG_CATEGORY_CUSTOM, "%s", message.c_str());
        }

        template<typename ...Args>
        void Error(std::string message, Args&&... args)
        {
            message = Format(message, std::forward<Args>(args)...);
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", message.c_str());
        }

        template<typename ...Args>
        void Critical(std::string message, Args&&... args)
        {
            message = Format(message, std::forward<Args>(args)...);
            SDL_LogCritical(SDL_LOG_CATEGORY_CUSTOM, "%s", message.c_str());
        }

        template<typename ...Args>
        void Verbose(std::string message, Args&&... args)
        {
            message = Format(message, std::forward<Args>(args)...);
            SDL_LogVerbose(SDL_LOG_CATEGORY_CUSTOM, "%s", message.c_str());
        }

        template<typename ...Args>
        void Debug(std::string message, Args&&... args)
        {
            #ifdef OSSIUM_DEBUG
            message = Format(message, std::forward<Args>(args)...);
            SDL_LogDebug(SDL_LOG_CATEGORY_CUSTOM, "%s", message.c_str());
            #endif // DEBUG
        }

    };

    extern Logger Log;

}

#endif // LOGGING_H
