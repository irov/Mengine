#	include "ConfigManager.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/PlatformInterface.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ConfigService, Menge::ConfigManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	ConfigManager::ConfigManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::_initialize()
	{
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlatformInterface ) == true )
		{
			const Tags & platformTags = PLATFORM_SERVICE( m_serviceProvider )
				->getPlatformTags();

			m_platformTags = platformTags;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::loadConfig( const ConstString & _fileGroup, const ConstString & _applicationPath )
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
	bool ConfigManager::addValue( const Char * _section, const Char * _key, const Char * _value )
	{
		return IniUtil::addIniValue( m_ini, _section, _key, _value, m_serviceProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::setValue( const Char * _section, const Char * _key, const Char * _value )
	{
		return IniUtil::setIniValue( m_ini, _section, _key, _value, m_serviceProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		template<class T>
		static T s_getValueT( ServiceProviderInterface * _serviceProvider, const IniUtil::IniStore & _ini, const Tags & _platform, const Char * _section, const Char * _key, T _default )
		{
			stdex::array_string<128> platform_section;
			platform_section.append( _section );
			
			const TVectorConstString & tags = _platform.getTags();

			for( TVectorConstString::const_iterator
				it = tags.begin(),
				it_end = tags.end();
			it != it_end;
			++it )
			{
				const ConstString & tag = *it;

				platform_section.append( '-' );
				platform_section.append( tag );

				T value;
				if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, value, _serviceProvider ) == true )
				{
					return value;
				}
			}

			T value;
			if( IniUtil::getIniValue( _ini, _section, _key, value, _serviceProvider ) == true )
			{
				return value;
			}

			return _default;
		}
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		static void s_calcValueT( ServiceProviderInterface * _serviceProvider, const IniUtil::IniStore & _ini, const Tags & _platform, const Char * _section, const Char * _key, T & _value )
		{
			stdex::array_string<128> platform_section;
			platform_section.append( _section );

			const TVectorConstString & tags = _platform.getTags();

			for( TVectorConstString::const_iterator
				it = tags.begin(),
				it_end = tags.end();
			it != it_end;
			++it )
			{
				const ConstString & tag = *it;

				platform_section.append( '-' );
				platform_section.append( tag );

				if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, _value, _serviceProvider ) == false )
				{
					continue;
				}
			}

			IniUtil::getIniValue( _ini, _section, _key, _value, _serviceProvider );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::getValue( const Char * _section, const Char * _key, bool _default ) const
	{
		return Helper::s_getValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	int32_t ConfigManager::getValue( const Char * _section, const Char * _key, int32_t _default ) const
	{
		return Helper::s_getValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ConfigManager::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
	{
		return Helper::s_getValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	float ConfigManager::getValue( const Char * _section, const Char * _key, float _default ) const
	{
		return Helper::s_getValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	String ConfigManager::getValue( const Char * _section, const Char * _key, const Char * _default ) const
	{
		return Helper::s_getValueT<String>( m_serviceProvider, m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	WString ConfigManager::getValue( const char * _section, const char * _key, const WChar * _default ) const
	{
		return Helper::s_getValueT<WString>( m_serviceProvider, m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString ConfigManager::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
	{
		return Helper::s_getValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	Resolution ConfigManager::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
	{
		return Helper::s_getValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorAspectRatioViewports & _value ) const
	{
		Helper::s_calcValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorWString & _value ) const
	{
		Helper::s_calcValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorString & _value ) const
	{
		Helper::s_calcValueT( m_serviceProvider, m_ini, m_platformTags, _section, _key, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getSection( const Char * _section, TMapParams & _params ) const
	{
		IniUtil::getIniAllSettings( m_ini, _section, _params, m_serviceProvider );
	}
}
