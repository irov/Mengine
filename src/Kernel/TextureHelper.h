#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR uint32_t getTexturePow2( uint32_t n )
        {
            --n;
            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;
            ++n;

            return n;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR uint32_t getTextureLog2( uint32_t n )
        {
            for( uint32_t index = 0; index != 32; ++index )
            {
                if( n & (1 << index) )
                {
                    return index;
                }
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR bool isTexturePow2( uint32_t n )
        {
            return !(n & (n - 1));
        }
        //////////////////////////////////////////////////////////////////////////
    }
}