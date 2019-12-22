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
#ifndef TYPEINFERENCE_H
#define TYPEINFERENCE_H

#include <sstream>
#include <type_traits>
#include <any>

#include "helpermacros.h"

using namespace std;

namespace Ossium
{

    inline namespace Utilities
    {

        ///
        /// SFINAE method/type detection
        /// Based primarily on work by Jean Guegant
        /// http://jguegant.github.io/blogs/tech/sfinae-introduction.html
        ///

        #define DETECT_METHOD(METHOD_NAME)                                                                                                                      \
        template<class T>                                                                                                                                       \
        struct OSSIUM_EDL has_##METHOD_NAME                                                                                                                     \
        {                                                                                                                                                       \
            /** decltype() returns a bool type if the type to check has the method, otherwise substitution fails and the alternative method is used instead. **/\
            template<typename TypeToCheck>                                                                                                                      \
            static constexpr decltype(declval<TypeToCheck>().METHOD_NAME(), bool())                                                                             \
            check(int)                                                                                                                                          \
            {                                                                                                                                                   \
                return true;                                                                                                                                    \
            }                                                                                                                                                   \
                                                                                                                                                                \
            /** If SFINAE takes place, this is the ultimate fallback method as it takes any type. **/                                                           \
            template<typename SubstitutionFailedType>                                                                                                           \
            static constexpr bool check(...)                                                                                                                    \
            {                                                                                                                                                   \
                return false;                                                                                                                                   \
            }                                                                                                                                                   \
                                                                                                                                                                \
            /** We have to pass something to the check() method such that it takes precedence over the variadic method, hence passing an int. **/               \
            static constexpr bool value = check<T>(int());                                                                                                      \
                                                                                                                                                                \
        }

        #define DETECT_METHOD_P(METHOD_NAME, PARAMTYPE)                                                                                                         \
        template<class T>                                                                                                                                       \
        struct OSSIUM_EDL has_##METHOD_NAME                                                                                                                                \
        {                                                                                                                                                       \
            template<typename TypeToCheck>                                                                                                                      \
            static constexpr decltype(declval<TypeToCheck>().METHOD_NAME(declval<PARAMTYPE>()), bool())                                                         \
            check(int)                                                                                                                                          \
            {                                                                                                                                                   \
                return true;                                                                                                                                    \
            }                                                                                                                                                   \
                                                                                                                                                                \
            template<typename SubstitutionFailedType>                                                                                                           \
            static constexpr bool check(...)                                                                                                                    \
            {                                                                                                                                                   \
                return false;                                                                                                                                   \
            }                                                                                                                                                   \
                                                                                                                                                                \
            static constexpr bool value = check<T>(int());                                                                                                      \
                                                                                                                                                                \
        }

        template<class T>
        struct is_range_erasable
        {
            template<typename TypeToCheck>
            static constexpr decltype(declval<TypeToCheck>().erase(declval<typename TypeToCheck::iterator>(), declval<typename TypeToCheck::iterator>()), bool())
            check(int)
            {
                return true;
            }

            template<typename SubstitutionFailedType>
            static constexpr bool check(...)
            {
                return false;
            }

            static constexpr bool value = check<T>(int());

        };

        inline namespace Internal
        {

            typedef char DoesNotHave;
            typedef char DoesHave[2];

            DoesNotHave operator<<(const ostream&, const any&);
            DoesNotHave operator>>(const ostream&, const any&);

            DoesHave& CheckInsertionOp(ostream&);
            DoesNotHave CheckInsertionOp(DoesNotHave);

        }

        template<typename T>
        struct is_insertable
        {
            static ostream& s;
            const static T& t;
            const static bool value = sizeof(CheckInsertionOp(s << t)) == sizeof(DoesHave);
        };

        template<typename T>
        struct is_streamable
        {
            static ostream& s;
            const static T& t;
            const static bool value = sizeof(CheckInsertionOp(s >> t)) == sizeof(DoesHave);
        };

        template<typename T, typename U = void>
        struct is_key_value_map : public false_type
        {
        };

        template<typename T>
        struct is_key_value_map<T, void_t<typename T::key_type, typename T::mapped_type, decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>> : public true_type
        {
        };

        DETECT_METHOD(ToString);

        DETECT_METHOD_P(FromString, string);

    }

}

#endif // TYPEINFERENCE_H
