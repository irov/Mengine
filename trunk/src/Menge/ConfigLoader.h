#	pragma once

#	include "ConfigFile/ConfigFile.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"
#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class ServiceProviderInterface;

    class ConfigLoader
	{
	public:
		ConfigLoader( ServiceProviderInterface * _serviceProvider );

	public:
		bool loadFile( const ConstString& _fileSystemName, const FilePath& _filename );

	public:
		bool getSetting( const WString & _section, const WString & _key, WString & _value ) const;		
		bool getSetting( const WString & _section, const WString & _key, ConstString & _value ) const;

	public:
		bool getSetting( const WString & _section, const WString & _key, WString & _value, const WString & _default ) const;
		bool getSetting( const WString & _section, const WString & _key, ConstString & _value, const ConstString & _default ) const;

	public:
		bool getSetting( const WString & _section, const WString & _key, bool & _value ) const;
		bool getSetting( const WString & _section, const WString & _key, int & _value ) const;

		bool getSetting( const WString & _section, const WString & _key, Resolution & _value ) const;
		bool getSetting( const WString & _section, const WString & _key, Viewport & _value ) const;
		

	public:
		bool getSettings( const WString & _section, const WString & _key, TVectorWString & _values ) const;
		bool getAllSettings( const WString& _section, TMapWString & _values );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ConfigFile m_config;
	};
}