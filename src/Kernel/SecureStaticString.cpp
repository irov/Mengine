#include "SecureStaticString.h"

namespace Mengine
{
    namespace Helper
    {
        void unsecureStaticString( const uint32_t * _data, Char * const _str )
        {
            uint32_t size = _data[0];
            uint32_t token = _data[1];

            Char * str = _str;

            for( uint32_t index = 0; index != size; ++index )
            {
                uint32_t d = _data[index + 2];
                *(str++) = (char)((d) ^ (token = ((token * 1103515245) + 12345) & 0x7fffffff));
            }

            *(str++) = '\0';
        }
    }
}