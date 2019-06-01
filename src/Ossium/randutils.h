#ifndef RANDUTILS_H
#define RANDUTILS_H

#include <functional>
#include <random>

#include "vector.h"

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
        structs::Vector Vector(structs::Vector min = {0, 0}, structs::Vector max = {1, 1});

        /// Returns a random unit vector.
        structs::Vector UnitVector();

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
