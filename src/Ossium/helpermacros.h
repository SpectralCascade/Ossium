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

    #define DEBUG_ASSERT(CONDITION, FAIL_MESSAGE)                       \
            if (!(CONDITION))                                           \
            {                                                           \
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, FAIL_MESSAGE);    \
                SDL_assert(false);                                      \
            }                                                           \
            SDL_assert(true)


}

#endif // HELPERMACROS_H
