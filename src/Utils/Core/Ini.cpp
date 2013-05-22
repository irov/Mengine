#   include "Ini.h"

#   include "Interface/UnicodeInterface.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    bool writeIniSection( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _file, const char * _section, size_t _sectionSize )
    {        
        (void)_serviceProvider;

        _file->write( _section, _sectionSize );
        _file->write( "\n", sizeof("\n") - 1 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool writeIniSetting( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _file, const ConstString & _key, const WString & _value )
    {
        _file->write( _key.c_str(), _key.size() );
        _file->write( " = ", sizeof(" = ") - 1 );

        char utf8_value[512];

        size_t utf8_size = 0;

        if( UNICODE_SERVICE(_serviceProvider)
            ->unicodeToUtf8( _value.c_str(), _value.size(), utf8_value, 512, &utf8_size ) == false )
        {
            return false;
        }

        _file->write( utf8_value, utf8_size );
        _file->write( "\n", sizeof("\n") - 1 );

        return true;
    }
}