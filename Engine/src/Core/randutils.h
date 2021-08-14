/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#ifndef RANDUTILS_H
#define RANDUTILS_H

#include <functional>
#include <random>

#include "coremaths.h"

namespace Ossium
{

    class OSSIUM_EDL Rand
    {
    public:
        Rand();
        Rand(int seed);
        ~Rand();

        /// Sets the RNG seed, also resetting the generator.
        void Seed(int seed);

        /// Returns a random integer (inclusive-inclusive).
        int Int(int min = 0, int max = 1);

        /// Returns a random float (inclusive-exclusive).
        float Float(float min = 0, float max = 1);

        /// Returns a random vector within the given range.
        Ossium::Vector2 Vector2(Ossium::Vector2 min = {0, 0}, Ossium::Vector2 max = {1, 1});

        /// Returns a random unit vector.
        Ossium::Vector2 UnitVector2();

    private:
        std::random_device rdev;
        std::mt19937* rng;

    };

    template<class T>
    T* PickRandom(std::vector<T>& data, Rand* rng = nullptr)
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
