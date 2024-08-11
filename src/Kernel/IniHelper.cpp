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
            Char value_str[64] = {'\0'};
            if( Helper::stringalized( _value, value_str, 63 ) == false )
            {
                return false;
            }

            bool successful = Helper::writeIniSetting( _stream, _key, value_str );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, float _value )
        {
            Char value_str[64] = {'\0'};
            if( Helper::stringalized( _value, value_str, 63 ) == false )
            {
                return false;
            }

            bool successful = Helper::writeIniSetting( _stream, _key, value_str );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const Char * _value, size_t _size )
        {
            size_t len = StdString::strlen( _key );

            _stream->write( _key, len );
            _stream->write( " = ", MENGINE_STATIC_STRING_LENGTH( " = " ) );
            _stream->write( _value, _size );
            _stream->write( "\n", MENGINE_STATIC_STRING_LENGTH( "\n" ) );

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
        bool writeIniSection( const OutputStreamInterfacePtr & _stream, const Char * _section )
        {
            size_t len = StdString::strlen( _section );

            _stream->write( _section, len );
            _stream->write( "\n", MENGINE_STATIC_STRING_LENGTH( "\n" ) );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}