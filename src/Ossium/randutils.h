#ifndef RANDUTILS_H
#define RANDUTILS_H

#include <functional>
#include <random>

using namespace std;

namespace Ossium
{

    class Rand
    {
    public:
        Rand();
        ~Rand();

        int Int(int min = 0, int max = 1);

    private:
        random_device rdev;
        mt19937* rng;

    };

    template<class T>
    T* Pick(vector<T>& data, function<bool(T&)> picker)
    {
        for (auto& item : data)
        {
            if (picker(item))
            {
                return &item;
            }
        }
        return nullptr;
    }

    template<class T>
    T* PickRandom(vector<T>& data)
    {
        if (!data.empty())
        {
            unsigned int index = Rand().Int(0, data.size() - 1);
            return &data[index];
        }
        return nullptr;
    }

}

#endif // RANDUTILS_H
