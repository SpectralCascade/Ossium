#include "stringconvert.h"

namespace Ossium
{

    inline namespace Utilities
    {

        string ToString(...)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "ToString reached the variadic function!");
            return "(ToString() not implemented for type)";
        }

        void FromString(...)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_ASSERT, "FromString reached the variadic function!");
        }

    }

}
