#   pragma once

#   include "stdex/ini.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/ServiceInterface.h"
#   include "Interface/StreamInterface.h"

#	include "Core/ConstString.h"
#   include "Core/FilePath.h"
#	include "Core/Resolution.h"
#	include "Core/Viewport.h"
#	include "Core/AspectRatioViewports.h"
#	include "Core/ColourValue.h"

#	ifndef MENGINE_INI_BUFFER_SIZE 
#	define MENGINE_INI_BUFFER_SIZE 16384
#	endif

namespace Menge
{
    namespace IniUtil
    {
		//////////////////////////////////////////////////////////////////////////
		struct IniStore
			: public stdex::ini
		{			
			char buff[MENGINE_INI_BUFFER_SIZE];
		};
		//////////////////////////////////////////////////////////////////////////
		bool loadIni( IniStore & _ini, const InputStreamInterfacePtr & _input, ServiceProviderInterface * _serviceProvider );
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, WString & _value, ServiceProviderInterface * _serviceProvider ); 
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, String & _value, ServiceProviderInterface * _serviceProvider );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, FilePath & _value, ServiceProviderInterface * _serviceProvider );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Resolution & _value, ServiceProviderInterface * _serviceProvider );
		bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, ColourValue & _value, ServiceProviderInterface * _serviceProvider );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, bool & _value, ServiceProviderInterface * _serviceProvider );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, size_t & _value, ServiceProviderInterface * _serviceProvider );
		bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, float & _value, ServiceProviderInterface * _serviceProvider );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorString & _values, ServiceProviderInterface * _serviceProvider );
		bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorConstString & _values, ServiceProviderInterface * _serviceProvider );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorWString & _values, ServiceProviderInterface * _serviceProvider );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorAspectRatioViewports & _values, ServiceProviderInterface * _serviceProvider );
        bool getIniAllSettings( const IniStore & _ini, const Char * _section, TMapParams & _values, ServiceProviderInterface * _serviceProvider );
		//////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _file, const char * _key, const WString & _value );
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
}