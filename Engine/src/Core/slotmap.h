#ifndef SLOTMAP_H
#define SLOTMAP_H

#include <vector>

namespace Ossium
{

    /// Constant time insert*, erase, lookup and O(n) iteration within contiguous memory.
    /// See https://www.youtube.com/watch?v=SHaAR7XPtNU for details.
    template<typename T>
    class SlotMap
    {
    public:
        SlotMap() = default;
        SlotMap(unsigned int size)
        {
            Reserve(size);
        }

        void Reserve(unsigned int size)
        {
            data.reserve(size);
            indexMap.reserve(size);
            indices.reserve(size);
        }

        void Insert(const T& data)
        {
            indices.
        }

    private:
        /// The data array.
        std::vector<T> data;

        /// Each value in the indices array must have an index corresponding to a data element.
        struct IndexGen
        {
            unsigned int index;
            unsigned int generation;
        };

        /// Indices array with generation counting, such that invalid keys cannot be used.
        /// This also contains a free list (a variant of a linked list).
        /// See this link for a good explanation: https://www.youtube.com/watch?v=Jae7Zl2iHic
        std::vector<IndexGen> indices;

        std::vector<IndexGen>::iterator freeListHead;
        std::vector<IndexGen>::iterator freeListTail;

        /// Mapping between the indices and the data for constant-time removal.
        std::vector<unsigned int> indexMap;

    };

}

#endif // SLOTMAP_H
