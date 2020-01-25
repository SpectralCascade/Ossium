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

        /// Removes an element from the cache
        void Erase(const T& data)
        {
            auto itr = accessLookup.find(data);
            if (itr != accessLookup.end())
            {
                cache.erase(itr->second);
                accessLookup.erase(itr);
            }
        }

        void PopLRU()
        {
            if (!cache.empty())
            {
                Erase(cache.back());
            }
        }

        /// Returns a copy of the least recently used value.
        T GetLRU()
        {
            return cache.back();
        }

        /// Moves the data to the top of the cache.
        void Access(const T& data)
        {
            auto itr = accessLookup.find(data);
            if (itr != accessLookup.end())
            {
                // Found, just move to the top
                cache.erase(itr->second);
            }
            cache.push_front(data);
            accessLookup[data] = cache.begin();
        }

        unsigned int Size()
        {
            return accessLookup.empty() ? 0 : accessLookup.size();
        }

    private:
        list<T> cache;
        unordered_map<T, typename list<T>::iterator> accessLookup;

    };

}

#endif // LRUCACHE_H
