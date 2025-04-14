#include "FourCCHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        StaticString<4> getFourCCString( uint32_t _fourCC )
        {
            StaticString<4> fourCCString;
            fourCCString.change( 0, (_fourCC >> 24) & 0xFF );
            fourCCString.change( 1, (_fourCC >> 16) & 0xFF );
            fourCCString.change( 2, (_fourCC >> 8) & 0xFF );
            fourCCString.change( 3, (_fourCC >> 0) & 0xFF );

            return fourCCString;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}