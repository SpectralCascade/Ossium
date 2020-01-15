#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <list>
#include <unordered_map>

using namespace std;

namespace Ossium
{

    template<typename T>
    class LRUCache
    {
    public:
        /// Clears the cache
        void Clear()
        {
            accessLookup.clear();
            cache.clear();
        }

        void Erase(const T& data)
        {
            auto itr = accessLookup.find(data);
            if (itr != accessLookup.end())
            {
                cache.erase(itr.second);
                accessLookup.erase(itr);
            }
        }

        /// Moves the data to the top of the cache. If Size() >= maxSize, the element at the bottom of the cache
        /// is removed and returned. Otherwise returns nullptr.
        T Access(const T& data, unsigned int maxSize, T notRemovedValue)
        {
            return Access(data, Size() >= maxSize);
        }

        /// Moves the data to the top of the cache. If isFull == true, removes a reference and returns a pointer to it.
        /// Otherwise returns nullptr.
        T Access(const T& data, bool isFull, T notRemovedValue)
        {
            T removed = notRemovedValue;
            auto itr = accessLookup.find(data);
            if (itr != accessLookup.end())
            {
                // Found, just move to the top
                cache.erase(itr.second);
            }
            else if (isFull)
            {
                removed = cache.back();
                Erase(removed);
            }
            cache.push_front(data);
            return removed;
        }

        unsigned int Size()
        {
            return accessLookup.empty() ? 0 : accessLookup.size();
        }

    private:
        list<T> cache;
        unordered_map<T, list<typename T>::iterator> accessLookup;

    };

}

#endif // LRUCACHE_H
