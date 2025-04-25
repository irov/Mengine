#pragma once

#include "Kernel/String.h"

namespace Mengine
{
    namespace Helper
    {
        void makeUID( uint32_t _length, Char * const _uid );
        void makeUIDSeed( uint64_t _seed, uint32_t _length, Char * const _uid );
    }
}