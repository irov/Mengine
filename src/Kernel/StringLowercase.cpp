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

            while( (*dest++ = (Char)StdCType::tolower( *_source++ )) != '\0' );
        }
        //////////////////////////////////////////////////////////////////////////
        void stringUppercase( const Char * _source, Char * const _dest )
        {
            Char * dest = _dest;

            while( (*dest++ = (Char)StdCType::toupper( *_source++ )) != '\0' );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}