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

    Structs::Vector Rand::Vector(Structs::Vector min, Structs::Vector max)
    {
        return (Structs::Vector){Float(min.x, max.x), Float(min.y, max.y)};
    }

    Structs::Vector Rand::UnitVector()
    {
        Structs::Vector u = {1, 0};
        u.Rotate(Float(0, 360));
        return u;
    }

}
