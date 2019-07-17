#include "IniHelper.h"

#include <string.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const Char * _value, size_t _size )
        {
            size_t len = ::strlen( _key );
            _stream->write( _key, len );
            _stream->write( " = ", sizeof( " = " ) - 1 );
            _stream->write( _value, _size );
            _stream->write( "\n", sizeof( "\n" ) - 1 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const ConstString & _value )
        {
            bool result = writeIniSetting( _stream, _key, _value.c_str(), _value.size() );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const String & _value )
        {
            bool result = writeIniSetting( _stream, _key, _value.c_str(), _value.size() );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSection( const OutputStreamInterfacePtr & _stream, const Char * _section, uint32_t _sectionSize )
        {
            _stream->write( _section, _sectionSize );
            _stream->write( "\n", sizeof( "\n" ) - 1 );

            return true;
        }
    }
}