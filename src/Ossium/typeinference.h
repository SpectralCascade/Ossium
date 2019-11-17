#ifndef TYPEINFERENCE_H
#define TYPEINFERENCE_H

#include <sstream>
#include <type_traits>
#include <any>

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
        struct has_##METHOD_NAME                                                                                                                                \
        {                                                                                                                                                       \
            template<typename TypeToCheck>                                                                                                                      \
            /** decltype() returns a bool type if the type to check has the method, otherwise substitution fails and the alternative method is used instead. **/\
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
        struct has_##METHOD_NAME                                                                                                                                \
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
