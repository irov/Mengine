#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/StreamInterface.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    bool writeIniSetting( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _file, const ConstString & _key, const WString & _value );
    //////////////////////////////////////////////////////////////////////////
    bool writeIniSection( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _file, const char * _section, size_t _sectionSize );
    //////////////////////////////////////////////////////////////////////////
    template<class T, size_t I_Size>
    static bool writeIniSection( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _file, T (& _section) [I_Size] )
    {        
        (void)_serviceProvider;

        bool result = writeIniSection( _serviceProvider, _file, _section, sizeof(_section) - 1 );

        return result;
    }
}