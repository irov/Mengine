#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        bool generateRandomSeed( uint64_t * const _seed );
        bool generateRandomHexadecimal( size_t _length, Char * const _hexadecimal, bool _lowercase );
    }
}