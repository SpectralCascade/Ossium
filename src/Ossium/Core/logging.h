#ifndef LOGGING_H
#define LOGGING_H

#include "stringconvert.h"

namespace Ossium
{

    class OSSIUM_EDL Logger
    {
    public:
        template<typename ...Args>
        void Info(string message, Args&&... args)
        {
            message = Format(message, forward<Args>(args)...);
            SDL_LogInfo(SDL_LOG_CATEGORY_CUSTOM, message.c_str());
        }

        template<typename ...Args>
        void Warning(string message, Args&&... args)
        {
            message = Format(message, forward<Args>(args)...);
            SDL_LogWarn(SDL_LOG_CATEGORY_CUSTOM, message.c_str());
        }

        template<typename ...Args>
        void Error(string message, Args&&... args)
        {
            message = Format(message, forward<Args>(args)...);
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, message.c_str());
        }

        template<typename ...Args>
        void Critical(string message, Args&&... args)
        {
            message = Format(message, forward<Args>(args)...);
            SDL_LogCritical(SDL_LOG_CATEGORY_CUSTOM, message.c_str());
        }

        template<typename ...Args>
        void Verbose(string message, Args&&... args)
        {
            message = Format(message, forward<Args>(args)...);
            SDL_LogVerbose(SDL_LOG_CATEGORY_CUSTOM, message.c_str());
        }

        static Logger& EngineLog();

    private:
        static Logger* engineLog;

    };

}

#endif // LOGGING_H
