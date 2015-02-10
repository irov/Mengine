#	pragma once

#	include "Interface/ConfigInterface.h"

#   include "Core/IniUtil.h"

namespace Menge
{
	class ConfigManager
		: public ConfigServiceInterface
	{
	public:
		ConfigManager();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize( const ConstString & _platformName ) override;
		void finalize() override;

	public:
		bool loadConfig( const ConstString & _fileGroup, const ConstString & _applicationPath ) override;
		
	public:
		void addValue( const Char * _section, const Char * _key, const Char * _value ) override;
		void setValue( const Char * _section, const Char * _key, const Char * _value ) override;

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
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_platformName;

		IniUtil::IniStore m_ini;

		struct ModifySetting
		{
			const char * section;
			const char * key;

			String value;
		};

		typedef stdex::vector<ModifySetting> TVectorModifySetting;
		TVectorModifySetting m_modifySettings;
	};
}


