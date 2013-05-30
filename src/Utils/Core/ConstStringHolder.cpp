#	include "ConstStringHolder.h"

namespace Menge
{    
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////
        template<>
        int cs_strcmp<wchar_t>( const wchar_t * _left, const wchar_t * _right )
        {
            return wcscmp( _left, _right );
        }
        //////////////////////////////////////////////////////////////////////////			
        template<>
        size_t cs_make_hash<wchar_t>( const wchar_t * _data )
        {
            const wchar_t * str = _data;

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