#include "IniHelper.h"

#include "Kernel/Stringalized.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, uint32_t _value )
        {
            Char value_str[64];
            Helper::stringalized( _value, value_str, 64 );

            bool successful = Helper::writeIniSetting( _stream, _key, value_str );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, float _value )
        {
            Char value_str[64];
            Helper::stringalized( _value, value_str, 64 );

            bool successful = Helper::writeIniSetting( _stream, _key, value_str );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const Char * _value, size_t _size )
        {
            size_t len = MENGINE_STRLEN( _key );

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