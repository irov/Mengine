#pragma once

#include "Config/Typedef.h"
#include "Config/StdBit.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR uint32_t getTexturePow2( uint32_t _value )
        {
            if( _value == 0 )
            {
                return 0;
            }

            constexpr uint32_t maxPowerOfTwo = uint32_t{1} << 31;

            if( _value > maxPowerOfTwo )
            {
                return 0;
            }

            uint32_t result = StdBit::bit_ceil( _value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR uint32_t getTextureLog2( uint32_t _value )
        {
            if( _value == 0 )
            {
                return 0;
            }

            uint32_t result = static_cast<uint32_t>(StdBit::countr_zero( _value ));

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_CONSTEXPR bool isTexturePow2( uint32_t _value )
        {
            if( _value == 0 )
            {
                return true;
            }

            bool result = StdBit::has_single_bit( _value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}