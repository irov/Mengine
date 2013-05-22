#	pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/StreamInterface.h"

#	include "SimpleIni.h"

#	include "Core/ConstString.h"
#	include "Core/String.h"

#	include "stdio.h"

namespace Menge
{	
    typedef CSimpleIniCaseA TIniParser;

	class ConfigFile
	{
	public:
		ConfigFile( ServiceProviderInterface * _serviceProvider );

	public:
		bool load( const InputStreamInterfacePtr & _file );
		bool save( const OutputStreamInterfacePtr & _file );

	public:
		bool getSetting( const char * _section, const ConstString & _key, WString & _value ) const;
		bool getSettings( const char * _section, const ConstString & _key, TVectorWString & _values ) const;
		
	public:
		bool getSettingUInt( const char * _section, const ConstString & _key, size_t & _value ) const;
		
	protected:      
        ServiceProviderInterface * m_serviceProvider;

		TIniParser m_ini;
	};

}	// namespace Menge
