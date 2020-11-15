#include "random_gen.h"

#include <random>
#include <iostream>

namespace rng_namespace
{
    //std::random_device rd;
    //std::mt19937 mt(rd());
    std::mt19937 mt(42);
    
    double getRandom(double min, double max)
    {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(mt);
    }

    double getChance()
    {
        return getRandom(0.0, 1.0);
    }

    int getRandomInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(mt);
    }
}