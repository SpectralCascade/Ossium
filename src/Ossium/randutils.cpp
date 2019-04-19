#include "randutils.h"

using namespace std;

namespace Ossium
{

    Rand::Rand()
    {
        rng = new mt19937(rdev());
    }

    Rand::~Rand()
    {
        delete rng;
    }

    int Rand::Int(int min, int max)
    {
        uniform_int_distribution<mt19937::result_type> generator(min, max);
        return generator(*rng);
    }

}
