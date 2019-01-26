#ifndef HELPERMACROS_H
#define HELPERMACROS_H

namespace ossium
{

    /// Convenience macro for declaring private copy and assignment constructors
    /// Note that you must always declare a constructor if you utilise this macro
    #define NOCOPY(TYPE)                    \
        private:                            \
            TYPE(const TYPE& src);          \
            TYPE operator=(const TYPE& src)

}

#endif // HELPERMACROS_H