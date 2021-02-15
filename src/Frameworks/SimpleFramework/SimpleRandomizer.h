#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    namespace Helper
    {
        void setSimpleRandomizerSeed( uint32_t _seed );

        uint32_t getSimpleRandomizerRandom( uint32_t _max );
        uint32_t getSimpleRandomizerRandomRange( uint32_t _min, uint32_t _max );

        int32_t getSimpleRandomizerRandomi( int32_t _max );
        int32_t getSimpleRandomizerRandomRangei( int32_t _min, int32_t _max );

        float getSimpleRandomizerRandomf( float _max );
        float getSimpleRandomizerRandomRangef( float _min, float _max );
    }
}
