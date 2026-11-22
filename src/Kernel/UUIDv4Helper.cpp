#include "UUIDv4Helper.h"

#include "Kernel/MemoryCopy.h"

#include "Config/StdCType.h"
#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool parseUUIDv4( const Char * _value, size_t _size, UUIDv4 * const _uuid )
        {
            if( _size != UUIDv4::size_data )
            {
                return false;
            }

            if( _value[14] != '4' )
            {
                return false;
            }

            for( size_t index = 0; index != UUIDv4::size_data; ++index )
            {
                Char ch = _value[index];

                if( index == 8 || index == 13 || index == 18 || index == 23 )
                {
                    if( ch != '-' )
                    {
                        return false;
                    }

                    continue;
                }

                if( StdCType::isxdigit( static_cast<unsigned char>(ch) ) == 0 )
                {
                    return false;
                }
            }

            if( StdString::strchr( "89aAbB", _value[19] ) == nullptr )
            {
                return false;
            }

            Helper::memoryCopy( _uuid->data, 0, _value, 0, UUIDv4::size_data );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
