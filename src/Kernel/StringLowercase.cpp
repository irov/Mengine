#include "StringLowercase.h"

#include "Config/StdCType.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void stringLowercase( const Char * _source, Char * const _dest )
        {
            Char * dest = _dest;

            while( (*dest++ = (Char)MENGINE_TOLOWER( *_source++ )) != '\0' );
        }
        //////////////////////////////////////////////////////////////////////////
        void stringUppercase( const Char * _source, Char * const _dest )
        {
            Char * dest = _dest;

            while( (*dest++ = (Char)MENGINE_TOUPPER( *_source++ )) != '\0' );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}