#	pragma once

#	include "Interface/ConfigInterface.h"

#   include "Core/Tags.h"
#   include "Core/IniUtil.h"

namespace Menge
{
	class ConfigManager
		: public ServiceBase<ConfigServiceInterface>
	{
	public:
		ConfigManager();

	public:
		bool _initialize() override;

	public:
		bool loadConfig( const ConstString & _fileGroup, const ConstString & _applicationPath ) override;
		
	public:
		bool addValue( const Char * _section, const Char * _key, const Char * _value ) override;
		bool setValue( const Char * _section, const Char * _key, const Char * _value ) override;

	public:
		bool getValue( const Char * _section, const Char * _key, bool _default ) const override;
		int32_t getValue( const Char * _section, const Char * _key, int32_t _default ) const override;
		uint32_t getValue( const Char * _section, const Char * _key, uint32_t _default ) const override;
		float getValue( const Char * _section, const Char * _key, float _default ) const override;
		String getValue( const Char * _section, const Char * _key, const Char * _default ) const override;
		WString getValue( const Char * _section, const Char * _key, const WChar * _default ) const override;
		ConstString getValue( const Char * _section, const Char * _key, const ConstString & _default ) const override;
		Resolution getValue( const Char * _section, const Char * _key, const Resolution & _default ) const override;

	public:
		void getValues( const Char * _section, const Char * _key, TVectorAspectRatioViewports & _value ) const override;
		void getValues( const Char * _section, const Char * _key, TVectorWString & _value ) const override;
		void getValues( const Char * _section, const Char * _key, TVectorString & _value ) const override;

	public:
		void getSection( const Char * _section, TMapParams & _params ) const override;

	protected:
		Tags m_platformTags;

		IniUtil::IniStore m_ini;
	};
}


