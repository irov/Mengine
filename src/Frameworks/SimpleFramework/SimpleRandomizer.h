#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    namespace Helper
    {
        void setSimpleRandomizerSeed( uint64_t _seed );

        uint32_t getSimpleRandomizerRandom32( uint32_t _max );
        uint32_t getSimpleRandomizerRandomRange32( uint32_t _min, uint32_t _max );

        int32_t getSimpleRandomizerRandom32i( int32_t _max );
        int32_t getSimpleRandomizerRandomRange32i( int32_t _min, int32_t _max );

        float getSimpleRandomizerRandomf( float _max );
        float getSimpleRandomizerRandomRangef( float _min, float _max );
    }
}
