#ifndef RANDUTILS_H
#define RANDUTILS_H

#include <functional>
#include <random>

#include "coremaths.h"

using namespace std;

namespace Ossium
{

    class Rand
    {
    public:
        Rand();
        Rand(int seed);
        ~Rand();

        /// Sets the RNG seed, also resetting the generator.
        void Seed(int seed);

        /// Returns a random integer.
        int Int(int min = 0, int max = 1);

        /// Returns a random float.
        float Float(float min = 0, float max = 1);

        /// Returns a random vector within the given range.
        Structs::Vector2 Vector2(Structs::Vector2 min = {0, 0}, Structs::Vector2 max = {1, 1});

        /// Returns a random unit vector.
        Structs::Vector2 UnitVector2();

    private:
        random_device rdev;
        mt19937* rng;

    };

    template<class T>
    T* PickRandom(vector<T>& data, Rand* rng = nullptr)
    {
        if (!data.empty())
        {
            unsigned int index = rng != nullptr ? rng->Int(0, data.size() - 1) : Rand().Int(0, data.size() - 1);
            return &data[index];
        }
        return nullptr;
    }

}

#endif // RANDUTILS_H
