#pragma once

#include "Config/Config.h"

#include <random>

namespace Mengine
{
    namespace StdRandom
    {
        using std::mt19937_64;
        using std::random_device;
        using std::seed_seq;

        using std::uniform_int_distribution;
        using std::uniform_real_distribution;
    }
}