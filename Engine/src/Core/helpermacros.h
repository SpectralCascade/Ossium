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
#ifndef HELPERMACROS_H
#define HELPERMACROS_H

namespace Ossium
{

    /// Convenience macro for declaring private copy and assignment constructors
    /// Note that you must always declare a constructor if you utilise this macro
    #define NOCOPY(TYPE)                    \
        private:                            \
            TYPE(const TYPE& src);          \
            TYPE operator=(const TYPE& src)

    /// Turns a code snippet into a string
    #define STRINGIFY(CODE) "CODE"

    #ifdef OSSIUM_DEBUG
    #define DEBUG_ASSERT(CONDITION, FAIL_MESSAGE)                                               \
            if (!(CONDITION))                                                                   \
            {                                                                                   \
                Log.Error("{0}:{1} {2}", __FILE__ , __LINE__ , FAIL_MESSAGE);   \
                SDL_assert(CONDITION);                                                          \
            }                                                                                   \
            SDL_assert(true)
    #else
    #define DEBUG_ASSERT(CONDITION, FAIL_MESSAGE)
    #endif

    /// Export Dynamic Link macro for creating shared library links.
    /// NOTE: DLL builds are not supported due to issues with static variable linkage.
    #if defined(OSSIUM_EXPORT_DLL) && defined(_WIN32)
    #   define OSSIUM_EDL __declspec(dllexport)
    #else
    #   define OSSIUM_EDL
    #endif

    /// This macro shouldn't be used by mortal souls. But here it is anyway.
    #define EVIL_CAST(VALUE, TO_TYPE) *(( TO_TYPE *)((void*) & VALUE ))

    template<typename ToType, typename FromType>
    inline const ToType& EvilCast(const FromType& value)
    {
        return EVIL_CAST(value, ToType);
    }

}

#endif // HELPERMACROS_H
