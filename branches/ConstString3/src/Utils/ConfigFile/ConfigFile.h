#	pragma once

#	include "SimpleIni.h"

#	include "Core/String.h"

namespace Menge
{
	class FileInputStreamInterface;
	class FileOutputStreamInterface;

	class ConfigFile
	{
	public:
		ConfigFile();

	public:
		bool load( FileInputStreamInterface* _file );
		bool save( FileOutputStreamInterface* _file );

	public:
		bool getSetting( const WString& _section, const WString& _key, WString & _value ) const;
		bool getSettings( const WString& _section, const WString& _key, TVectorWString & _values ) const;

	public:
		bool getSettingUInt( const WString& _section, const WString& _key, unsigned int & _value ) const;
		
	public:
		bool setSetting( const WString& _section, const WString& _key, const WString & _value );

	protected:
		CSimpleIniW m_ini;
	};

}	// namespace Menge
