#	pragma once

#	include "SimpleIni.h"

#	include "Core/String.h"

#	include "stdio.h"

namespace Menge
{
	class InputStreamInterface;
	class OutputStreamInterface;

	class ConfigFile
	{
	public:
		ConfigFile();

	public:
		bool load( InputStreamInterface* _file );
		bool save( OutputStreamInterface* _file );

	public:
		bool getSetting( const WString& _section, const WString& _key, WString & _value ) const;
		bool getSettings( const WString& _section, const WString& _key, TVectorWString & _values ) const;
		bool getAllSettings( const WString& _section, TMapWString & _values );

	public:
		bool getSettingUInt( const WString& _section, const WString& _key, size_t & _value ) const;
		
	public:
		bool setSetting( const WString& _section, const WString& _key, const WString & _value );

	protected:
		CSimpleIniCaseW m_ini;
	};

}	// namespace Menge
