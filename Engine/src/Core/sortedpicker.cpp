#include "sortedpicker.h"

namespace Ossium
{

    bool operator<(const SortedPickerItemPair& a, const SortedPickerItemPair& b)
    {
        return a.weight < b.weight;
    }

}
