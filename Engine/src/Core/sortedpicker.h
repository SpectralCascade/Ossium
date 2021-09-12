#ifndef SORTEDPICKER_H
#define SORTEDPICKER_H

#include <queue>
#include <limits>

namespace Ossium
{
    struct SortedPickerItemPair
    {
        float weight;
        size_t index;
    };

    bool operator<(const SortedPickerItemPair& a, const SortedPickerItemPair& b);
    
    // Template class used for picking weighted items from an array.
    template<typename T>
    class SortedPicker
    {
    public:
        SortedPicker(size_t preallocate_size = 0)
        {
            if (preallocate_size > 0)
            {
                items.reserve(preallocate_size);
            }
        }

        // Add an item with a particular weighting to the picker
        void Add(T item, float weight)
        {
            weightings.push((SortedPickerItemPair){ .weight = weight, .index = items.size() });
            items.push_back(item);
        }

        // Pick the top item
        T Pick()
        {
            return items[PickIndex()];
        }

        // Pick the index to the top item
        unsigned int PickIndex()
        {
            SortedPickerItemPair item = weightings.top();
            weightings.pop();
            return item.index;
        }

        // Return the top item without removing it from the weightings
        const T& Top()
        {
            return items[TopIndex()];
        }

        // Return the index of the top item
        unsigned int TopIndex()
        {
            return weightings.top().index;
        }

        // The number of item weighting pairs
        size_t Size()
        {
            return weightings.size();
        }

        // Clear the items
        void Clear()
        {
            items.clear();
            weightings = std::priority_queue<SortedPickerItemPair>();
        }

    private:
        std::priority_queue<SortedPickerItemPair> weightings;
        std::vector<T> items;
        
    };

}

#endif
