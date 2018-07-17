#include "ConfigManager.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Exception.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ConfigService, Mengine::ConfigManager );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{	
	//////////////////////////////////////////////////////////////////////////
	ConfigManager::ConfigManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ConfigManager::~ConfigManager()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::_initializeService()
	{
		if( SERVICE_EXIST( Mengine::PlatformInterface ) == true )
		{
			const Tags & platformTags = PLATFORM_SERVICE()
				->getPlatformTags();

			m_platformTags = platformTags;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void ConfigManager::_finalizeService()
    {
        m_platformTags.clear();
    }
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _applicationPath )
	{
		InputStreamInterfacePtr applicationInputStream = FILE_SERVICE()
			->openInputFile( _fileGroup, _applicationPath, false );

		if( applicationInputStream == nullptr )
		{
			LOGGER_ERROR("ConfigManager::loadConfig Invalid open application settings %s"
				, _applicationPath.c_str()
				);

			return false;
		}

		if( IniUtil::loadIni( m_ini, applicationInputStream ) == false )
		{
			LOGGER_ERROR("ConfigManager::loadConfig Invalid load application settings %s"
				, _applicationPath.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		template<class T>
		static T s_getValueT( const IniUtil::IniStore & _ini, const Tags & _platform, const Char * _section, const Char * _key, T _default )
		{
			stdex::array_string<128> platform_section;
			platform_section.append( _section );
			
			const TVectorConstString & tags = _platform.getTags();

            for( const ConstString & tag : tags )
			{
				platform_section.append( '-' );
				platform_section.append( tag );

				T value;
				if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, value ) == true )
				{
					return value;
				}
			}

			T value;
			if( IniUtil::getIniValue( _ini, _section, _key, value ) == true )
			{
				return value;
			}

			return _default;
		}
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		static void s_calcValueT( const IniUtil::IniStore & _ini, const Tags & _platform, const Char * _section, const Char * _key, T & _value )
		{
			stdex::array_string<128> platform_section;
			platform_section.append( _section );

			const TVectorConstString & tags = _platform.getTags();

            stdex::array_string<128> platform_section_found;

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( IniUtil::hasIniValue( _ini, platform_section.c_str(), _key ) == false )
                {
                    continue;
                }

                platform_section_found = platform_section;

                break;
            }

			if( platform_section_found.empty() == false )
			{
                IniUtil::getIniValue( _ini, platform_section_found.c_str(), _key, _value );

                return;
			}

			IniUtil::getIniValue( _ini, _section, _key, _value );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigManager::getValue( const Char * _section, const Char * _key, bool _default ) const
	{
		return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	int32_t ConfigManager::getValue( const Char * _section, const Char * _key, int32_t _default ) const
	{
		return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ConfigManager::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
	{
		return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	uint64_t ConfigManager::getValue( const Char * _section, const Char * _key, uint64_t _default ) const
	{
		return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	float ConfigManager::getValue( const Char * _section, const Char * _key, float _default ) const
	{
		return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
	}
    //////////////////////////////////////////////////////////////////////////
    double ConfigManager::getValue( const Char * _section, const Char * _key, double _default ) const
    {
        return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
    }
	//////////////////////////////////////////////////////////////////////////
	String ConfigManager::getValue( const Char * _section, const Char * _key, const Char * _default ) const
	{
		return Helper::s_getValueT<String>( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	WString ConfigManager::getValue( const char * _section, const char * _key, const WChar * _default ) const
	{
		return Helper::s_getValueT<WString>( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString ConfigManager::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
	{
		return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	FilePath ConfigManager::getValue( const Char * _section, const Char * _key, const FilePath & _default ) const
	{
		return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	Resolution ConfigManager::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
	{
		return Helper::s_getValueT( m_ini, m_platformTags, _section, _key, _default );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorAspectRatioViewports & _value ) const
	{
		Helper::s_calcValueT( m_ini, m_platformTags, _section, _key, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorWString & _value ) const
	{
		Helper::s_calcValueT( m_ini, m_platformTags, _section, _key, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getValues( const Char * _section, const Char * _key, TVectorString & _value ) const
	{
		Helper::s_calcValueT( m_ini, m_platformTags, _section, _key, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigManager::getSection( const Char * _section, TMapWParams & _params ) const
	{
		IniUtil::getIniAllSettings( m_ini, _section, _params );
	}
}
