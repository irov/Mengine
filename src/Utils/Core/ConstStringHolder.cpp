#	include "ConstStringHolder.h"

namespace Menge
{    
    namespace Detail
    {
        template<>
        int cs_strcmp<char>( const char * _left, const char * _right )
        {
            return strcmp( _left, _right );
        }
        //////////////////////////////////////////////////////////////////////////			
        template<>
        size_t cs_make_hash<char>( const char * _data )
        {
            const char * str = _data;

            size_t hash = 5381;
            int c;

            while(c = *str++)
            {
                hash = ((hash << 5) + hash) + c;
            }

            return hash;
        }
    }
}