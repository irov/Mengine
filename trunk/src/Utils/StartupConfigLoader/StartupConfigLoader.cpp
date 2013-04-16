#	include "StartupConfigLoader.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, WString & _value )
	{
		const WChar * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		_value.assign( w_value );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static SI_Error s_loadIniFile( CSimpleIniCaseW & _ini, InputStreamInterface * applicationInputStream )
	{
        size_t size = applicationInputStream->size();

		char * buffer = new char[size + 1];
		buffer[size] = 0;

        applicationInputStream->read( buffer, size );

		SI_Error iniError = _ini.LoadData( buffer );
		
		delete [] buffer;

		return iniError;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, String & _value )
	{
		const WChar * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}
		
		CSimpleIniCaseW::Converter converter = _ini.GetConverter();
		
		converter.ConvertToStore( w_value );
		_value = converter.Data();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, Resolution & _value )
	{
		const WChar * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		int width;
		int height;
		if( swscanf( w_value, L"%d;%d", &width, &height ) != 2 )
		{
			return false;
		}

		_value.setWidth( width );
		_value.setHeight( height );

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	//static bool s_getIniValue( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, Viewport & _value )
	//{
	//	const WChar * w_value = _ini.GetValue( _section, _key );

	//	if( w_value == NULL )
	//	{
	//		return false;
	//	}

	//	float left;
	//	float top;
	//	float right;
	//	float bottom;

	//	if( swscanf( w_value, L"%f;%f;%f;%f", &left, &top, &right, &bottom ) != 4 )
	//	{
	//		return false;
	//	}

	//	_value.begin.x = left;
	//	_value.begin.y = top;
	//	_value.end.x = right;
	//	_value.end.y = bottom;

	//	return true;
	//}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, bool & _value )
	{
		const WChar * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		int tmp_value;

		if( swscanf( w_value, L"%d", &tmp_value ) != 1 )
		{
			return false;
		}

		_value = tmp_value != 0;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, size_t & _value )
	{
		const WChar * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		size_t tmp_value;
		if( swscanf( w_value, L"%u", &tmp_value ) != 1 )
		{
			return false;
		}

		_value = tmp_value;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValues( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, TVectorWString & _values )
	{
		CSimpleIniCaseW::TNamesDepend values;
		if( _ini.GetAllValues( _section, _key, values ) == false )
		{
			return false;
		}

		for( CSimpleIniCaseW::TNamesDepend::const_iterator 
			it = values.begin(),
			it_end = values.end();
		it != it_end;
		++it )
		{
			_values.push_back( it->pItem );
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    static bool s_getIniValues( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, TVectorAspectRatioViewports & _values )
    {
        CSimpleIniCaseW::TNamesDepend values;
        if( _ini.GetAllValues( _section, _key, values ) == false )
        {
            return false;
        }

        for( CSimpleIniCaseW::TNamesDepend::const_iterator 
            it = values.begin(),
            it_end = values.end();
        it != it_end;
        ++it )
        {
            const WChar * w_value = it->pItem;

            AspectRatioViewports arv;
                        
            if( swscanf( w_value, L"%f;%f;%f;%f;%f;%f"
                , &arv.width
                , &arv.height
                , &arv.viewport.begin.x
                , &arv.viewport.begin.y
                , &arv.viewport.end.x
                , &arv.viewport.end.y
                ) != 6 )
            {
                return false;
            }

            _values.push_back( arv );
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool s_getIniAllSettings( const CSimpleIniCaseW & _ini, const WChar * _section, TMapWString & _values )
	{
		CSimpleIniCaseW::TNamesDepend values;
		if( _ini.GetAllKeys( _section, values ) == false )
		{
			return false;
		}

		for( CSimpleIniCaseW::TNamesDepend::const_iterator 
			it = values.begin(),
			it_end = values.end();
		it != it_end;
		++it )
		{
			WString val;
			s_getIniValue( _ini, _section, it->pItem, val );

			_values.insert( std::make_pair( it->pItem, val ) );
		}

		return true;
	}
	/////////////////////////////////////////////////////
	StartupConfigLoader::StartupConfigLoader( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
		, m_applicationIni(true, true, true)
		, m_gameIni(true, true, true)
		, m_resourcePackIni(true, true, true)
	{
	}
	/////////////////////////////////////////////////////
	StartupConfigLoader::~StartupConfigLoader()
	{
	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::load( const ConstString & _fileGroup, const FilePath & _applicationPath, StartupSettings & _settings )
	{
        InputStreamInterface * applicationInputStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, _applicationPath );

        if( applicationInputStream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid open application settings %s"
                , _applicationPath.c_str()
                );

            return false;
        }

		//SI_Error iniError = m_applicationIni.LoadFile( applicationSettingsPath.c_str() );

		SI_Error iniError = s_loadIniFile( m_applicationIni, applicationInputStream );

        applicationInputStream->destroy();
        applicationInputStream = NULL;
		
		if( iniError != 0 )
		{
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid load application settings %s"
                , _applicationPath.c_str()
                );

			return false;
		}

		const WChar * w_game_path = m_applicationIni.GetValue( L"Game", L"Path" );

		if( w_game_path == 0 )
		{
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Not found Game Path %s"
                , _applicationPath.c_str()
                );

			return false;
		}

		const WChar * w_resource_path = m_applicationIni.GetValue( L"Resource", L"Path" );

		if( w_resource_path == 0 )
		{
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Not found Resources Path %s"
                , _applicationPath.c_str()
                );

			return false;
		}

        String gameIniUtf8;
        if( Helper::unicodeToUtf8( m_serviceProvider, w_game_path, gameIniUtf8 ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid convert unicode to utf8 %ls"
                , w_game_path
                );

            return false;
        }

        FilePath gameIniPath = Helper::stringizeString( m_serviceProvider, gameIniUtf8 );

        InputStreamInterface * gameInputStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, gameIniPath );

        if( gameInputStream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid open game setting %ls"
                , w_game_path
                );

            return false;
        }
        
		if( this->loadGame_( gameInputStream, _settings ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid load game setting %ls"
                , w_game_path
                );
            			
			return false;
		}

        gameInputStream->destroy();

        String resourcesIniUtf8;
        if( Helper::unicodeToUtf8( m_serviceProvider, w_resource_path, resourcesIniUtf8 ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid convert unicode to utf8 %ls"
                , w_resource_path
                );

            return false;
        }

        FilePath resourcesIniPath = Helper::stringizeString( m_serviceProvider, resourcesIniUtf8 );

        InputStreamInterface * resourceInputStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, resourcesIniPath );
        		
        if( resourceInputStream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid open resourcesPack setting %ls"
                , w_resource_path
                );

            return false;
        }


		if( this->loadResourcePacks_( resourceInputStream, _settings ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid load resourcesPack setting %ls"
                , w_resource_path
                );

			return false;
		}

        resourceInputStream->destroy();

		return true;
	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::loadGame_( InputStreamInterface * _inputStream, StartupSettings & _settings )
	{
		SI_Error game_error = s_loadIniFile( m_gameIni, _inputStream );

		if( game_error != 0 )
		{
			return false;
		}

		_settings.alreadyRunning = false;

		_settings.applicationSettings.fixedContentResolution = true;
		_settings.applicationSettings.windowModeCheck = false;
		_settings.applicationSettings.fullscreen = true;
		_settings.applicationSettings.vsync = true;

		s_getIniValue( m_gameIni, L"Project", L"Name", _settings.projectName );
		s_getIniValue( m_gameIni, L"Project", L"Company", _settings.companyName );
		s_getIniValue( m_gameIni, L"Project", L"Codename", _settings.applicationSettings.projectCodename );
		s_getIniValue( m_gameIni, L"Locale", L"Default", _settings.defaultLocale );
		s_getIniValue( m_gameIni, L"Game", L"ContentResolution", _settings.applicationSettings.contentResolution );     
		s_getIniValue( m_gameIni, L"Game", L"FixedContentResolution", _settings.applicationSettings.fixedContentResolution );
		s_getIniValue( m_gameIni, L"Game", L"PersonalityModule", _settings.personalityModule );
		s_getIniValue( m_gameIni, L"Game", L"AlreadyRunning", _settings.alreadyRunning );
		s_getIniValue( m_gameIni, L"Game", L"WindowModeCheck", _settings.applicationSettings.windowModeCheck );
		s_getIniValue( m_gameIni, L"Window", L"Size", _settings.applicationSettings.windowResolution );
		s_getIniValue( m_gameIni, L"Window", L"Bits", _settings.applicationSettings.bits );
		s_getIniValue( m_gameIni, L"Window", L"Fullscreen", _settings.applicationSettings.fullscreen );
		s_getIniValue( m_gameIni, L"Window", L"VSync", _settings.applicationSettings.vsync );

        s_getIniValues( m_gameIni, L"Game", L"AspectRatioViewport", _settings.applicationSettings.aspectRatioViewports );

		s_getIniAllSettings( m_gameIni, L"Params", _settings.appParams );

		s_getIniValues( m_gameIni, L"Plugins", L"Name", _settings.plugins );
        s_getIniValues( m_gameIni, L"DevPlugins", L"Name", _settings.devPlugins );
        
		return true;
	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::loadResourcePacks_( InputStreamInterface * _inputStream, StartupSettings & _settings )
	{
		SI_Error error = s_loadIniFile( m_resourcePackIni, _inputStream );

		if( error != 0 )
		{
			return false;
		}

		TVectorWString resourcePacksSettings;
		s_getIniValues( m_resourcePackIni, L"GAME_RESOURCES", L"ResourcePack", resourcePacksSettings );

		for( TVectorWString::iterator
			it = resourcePacksSettings.begin(),
			it_end = resourcePacksSettings.end();
		it != it_end;
		++it )
		{
			const WString & resourcePack = *it;

			ResourcePackDesc pack;

			pack.preload = true;
			pack.type = "dir";

			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Name", pack.name );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Type", pack.type );            
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Path", pack.path );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Locale", pack.locale );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Platform", pack.platform );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Description", pack.description );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"PreLoad", pack.preload );						

			_settings.resourcePacks.push_back( pack );
		}

		TVectorWString languagePackSettings;
		s_getIniValues( m_resourcePackIni, L"GAME_RESOURCES", L"LanguagePack", languagePackSettings );

		for( TVectorWString::iterator
			it = languagePackSettings.begin(),
			it_end = languagePackSettings.end();
		it != it_end;
		++it )
		{
			const WString & resourcePack = *it;

			ResourcePackDesc pack;

			pack.preload = true;
			pack.type = "dir";			

			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Type", pack.type );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Name", pack.name );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Path", pack.path );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Locale", pack.locale );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Platform", pack.platform );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"Description", pack.description );
			s_getIniValue( m_resourcePackIni, resourcePack.c_str(), L"PreLoad", pack.preload );	

			_settings.languagePacks.push_back( pack );
		}

		return true;
	}
	/////////////////////////////////////////////////////

}