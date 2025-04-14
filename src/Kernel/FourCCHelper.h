#pragma once

#include "Kernel/StaticString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template <size_t N>
        StaticString<N> getFourCCString( const uint8_t * _fourCC )
        {
            StaticString<N> fourCCString;
            for( size_t index = 0; index != N; ++index )
            {
                fourCCString[index] = _fourCC[index];
            }

            return fourCCString;
        }
        //////////////////////////////////////////////////////////////////////////
        StaticString<4> getFourCCString( uint32_t _fourCC );
        //////////////////////////////////////////////////////////////////////////
    }
}