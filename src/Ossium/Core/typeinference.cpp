#include "typeinference.h"

namespace Ossium
{

    inline namespace Utilities
    {

        inline namespace Internal
        {

            DoesNotHave operator>>(const ostream&, const any&)
            {
                return DoesNotHave();
            }

            DoesNotHave operator<<(const ostream&, const any&)
            {
                return DoesNotHave();
            }

        }

    }

}
