/** COPYRIGHT NOTICE
 *  
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
#include "randutils.h"

using namespace std;

namespace Ossium
{

    Rand::Rand()
    {
        rng = new mt19937(rdev());
    }

    Rand::Rand(int seed)
    {
        rng = new mt19937(rdev());
        rng->seed(seed);
    }

    Rand::~Rand()
    {
        delete rng;
    }

    void Rand::Seed(int seed)
    {
        rng->seed(seed);
    }

    int Rand::Int(int min, int max)
    {
        uniform_int_distribution<int> generator(min, max);
        return generator(*rng);
    }

    float Rand::Float(float min, float max)
    {
        uniform_real_distribution<float> generator(min, max);
        return generator(*rng);
    }

    Ossium::Vector2 Rand::Vector2(Ossium::Vector2 min, Ossium::Vector2 max)
    {
        return (Ossium::Vector2){Float(min.x, max.x), Float(min.y, max.y)};
    }

    Ossium::Vector2 Rand::UnitVector2()
    {
        Ossium::Vector2 u = {1, 0};
        u.Rotate(Float(0, 360));
        return u;
    }

}
