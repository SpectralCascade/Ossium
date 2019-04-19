#ifndef RANDUTILS_H
#define RANDUTILS_H

#include <functional>

using namespace std;

namespace Ossium
{

    template<class T>
    T* Pick(vector<T>& data, function<bool(T&)> picker)
    {
        for (auto item : data)
        {
            if (picker(item))
            {
                return &item;
            }
        }
        return nullptr;
    }

}

#endif // RANDUTILS_H
