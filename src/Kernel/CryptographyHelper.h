#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        bool generateRandomSeed( uint64_t * const _seed );
        bool generateRandomHexadecimal( uint32_t _length, Char * const _hexadecimal, bool _lowercase );
    }
}