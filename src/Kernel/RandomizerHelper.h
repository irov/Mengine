#pragma once

#include "Interface/RandomizerInterface.h"

namespace Mengine
{
    namespace Helper
    {
        void randomizerShuffle( const RandomizerInterfacePtr & _randomizer, uint32_t * const _indices, uint32_t _count );
    }
}