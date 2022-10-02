#include "StringSplit.h"

namespace Mengine
{
    namespace Helper
    {
        void stringSplit( const Char * _in, const LambdaStringSplit & _lambda )
        {
            const Char * probe = _in;

            for( ;;)
            {
                if( *probe == '\0' )
                {
                    break;
                }

                while( *probe == ' ' ) probe++;

                if( *probe == '\0' )
                {
                    break;
                }

                const Char * next = probe;

                while( *next != ' ' && *next != '\0' ) next++;

                if( *next == '\0' )
                {
                    _lambda( probe, next );

                    break;
                }

                _lambda( probe, next );

                probe = next + 1;
            }
        }
    }
}