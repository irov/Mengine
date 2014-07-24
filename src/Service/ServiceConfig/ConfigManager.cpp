#	include "ConfigManager.h"

#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ConfigService, Menge::ConfigServiceInterface, Menge::ConfigManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	ConfigManager::ConfigManager()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ConfigManager::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::loadConfig( const ConstString & _fileGroup, const FilePath & _applicationPath )
	{
		InputStreamInterfacePtr applicationInputStream = 
			FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, _applicationPath, false );

		if( applicationInputStream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ConfigManager::loadConfig Invalid open application settings %s"
				, _applicationPath.c_str()
				);

			return false;
		}

		if( IniUtil::loadIni( m_ini, applicationInputStream, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ConfigManager::loadConfig Invalid load application settings %s"
				, _applicationPath.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::setValue( const Char * _section, const Char * _key, const Char * _value )
	{
		IniUtil::mergeIniValue( m_ini, _section, _key, _value, m_serviceProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::getValue( const Char * _section, const Char * _key, bool _default ) const
	{
		bool value = _default;
		IniUtil::getIniValue( m_ini, _section, _key, value, m_serviceProvider );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	int32_t ConfigManager::getValue( const Char * _section, const Char * _key, int32_t _default ) const
	{
		int32_t value = _default;
		IniUtil::getIniValue( m_ini, _section, _key, value, m_serviceProvider );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ConfigManager::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
	{
		uint32_t value = _default;
		IniUtil::getIniValue( m_ini, _section, _key, value, m_serviceProvider );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	float ConfigManager::getValue( const Char * _section, const Char * _key, float _default ) const
	{
		float value = _default;
		IniUtil::getIniValue( m_ini, _section, _key, value, m_serviceProvider );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	String ConfigManager::getValue( const Char * _section, const Char * _key, const Char * _default ) const
	{
		String value = _default;
		IniUtil::getIniValue( m_ini, _section, _key, value, m_serviceProvider );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	WString ConfigManager::getValue( const char * _section, const char * _key, const WChar * _default ) const
	{
		WString value = _default;
		IniUtil::getIniValue( m_ini, _section, _key, value, m_serviceProvider );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString ConfigManager::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
	{
		ConstString value = _default;
		IniUtil::getIniValue( m_ini, _section, _key, value, m_serviceProvider );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	Resolution ConfigManager::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
	{
		Resolution value = _default;
		IniUtil::getIniValue( m_ini, _section, _key, value, m_serviceProvider );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorAspectRatioViewports & _value ) const
	{
		IniUtil::getIniValue( m_ini, _section, _key, _value, m_serviceProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorWString & _value ) const
	{
		IniUtil::getIniValue( m_ini, _section, _key, _value, m_serviceProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorString & _value ) const
	{
		IniUtil::getIniValue( m_ini, _section, _key, _value, m_serviceProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getSection( const Char * _section, TMapParams & _params ) const
	{
		IniUtil::getIniAllSettings( m_ini, _section, _params, m_serviceProvider );
	}
}
