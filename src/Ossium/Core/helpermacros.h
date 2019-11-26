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

    #ifdef DEBUG
    #define DEBUG_ASSERT(CONDITION, FAIL_MESSAGE)                       \
            if (!(CONDITION))                                           \
            {                                                           \
                Logger::EngineLog().Error(FAIL_MESSAGE);                \
                SDL_assert(false);                                      \
            }                                                           \
            SDL_assert(true)
    #else
    #define DEBUG_ASSERT(CONDITION, FAIL_MESSAGE)
    #endif

    /// For exporting classes and functions to a shared library
    #define EXPORT_FUNC

    /// This macro shouldn't be used by mortal souls. But here it is anyway.
    #define EVIL_CAST(VALUE, TO_TYPE) *(( TO_TYPE *)((void*) & VALUE ))

    template<typename ToType, typename FromType>
    inline const ToType& EvilCast(const FromType& value)
    {
        return EVIL_CAST(value, ToType);
    }

}

#endif // HELPERMACROS_H
