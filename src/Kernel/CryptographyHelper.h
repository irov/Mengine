#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        bool generateRandomSeed( uint64_t * const _seed );
        bool generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, bool _lowercase );
    }
}