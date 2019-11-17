#include "stringconvert.h"
#include "logging.h"

namespace Ossium
{

    inline namespace Utilities
    {

        string ToString(...)
        {
            Logger::EngineLog().Warning("ToString reached the variadic function!");
            return "(ToString() not implemented for type)";
        }

        void FromString(...)
        {
            Logger::EngineLog().Warning("FromString reached the variadic function!");
        }

        void ToStrings(vector<string>& converted)
        {
        }

        vector<string> ToStrings()
        {
            return vector<string>();
        }

    }

}
