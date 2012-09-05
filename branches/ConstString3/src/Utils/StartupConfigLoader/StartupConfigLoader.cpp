#	include "StartupConfigLoader.h"

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
	static bool s_unicodeToUtf( const CSimpleIniCaseW & _ini, const WString & _source, String & _value )
	{
		CSimpleIniCaseW::Converter converter = _ini.GetConverter();
		
		converter.ConvertToStore( _source.c_str() );
		_value = converter.Data();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static SI_Error s_loadIniFile( CSimpleIniCaseW & _ini, const WString & _path )
	{
		String pathA;
		s_unicodeToUtf( _ini, _path, pathA );

		FILE * fp = fopen( pathA.c_str(), "rb" );
		
		if( fp == 0 )
		{
		  return SI_FILE;
		}
		
		fseek (fp , 0 , SEEK_END);
		int size = ftell (fp);
		rewind (fp);

		char * buffer = new char[size + 1];
		buffer[size] = 0;

		fread ( buffer, 1, size, fp );

		SI_Error iniError = _ini.LoadData( buffer );
		
		fclose(fp);
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
		
		WString valW(w_value);
		String valA;
		s_unicodeToUtf( _ini, valW, valA );

		int width;
		int height;
		if( sscanf( valA.c_str(), "%d;%d", &width, &height ) != 2 )
		{
			return false;
		}

		_value.setWidth( width );
		_value.setHeight( height );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, Viewport & _value )
	{
		const WChar * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		float left;
		float top;
		float right;
		float bottom;

		WString valW(w_value);
		String valA;
		s_unicodeToUtf( _ini, valW, valA );

		if( sscanf( valA.c_str(), "%f;%f;%f;%f", &left, &top, &right, &bottom ) != 4 )
		{
			return false;
		}

		_value.begin.x = left;
		_value.begin.y = top;
		_value.end.x = right;
		_value.end.y = bottom;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const WChar * _section, const WChar * _key, bool & _value )
	{
		const WChar * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		int tmp_value;

		WString valW(w_value);
		String valA;
		s_unicodeToUtf( _ini, valW, valA );

		if( sscanf( valA.c_str(), "%d", &tmp_value ) != 1 )
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

		WString valW(w_value);
		String valA;
		s_unicodeToUtf( _ini, valW, valA );

		size_t tmp_value;
		if( sscanf( valA.c_str(), "%u", &tmp_value ) != 1 )
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
	StartupConfigLoader::StartupConfigLoader()
		: m_applicationIni(false, true, true)
		, m_gameIni(false, true, true)
		, m_resourcePackIni(false, true, true)
		, m_error(L"")
	{
	}
	/////////////////////////////////////////////////////
	StartupConfigLoader::~StartupConfigLoader()
	{

	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::load( const WString& _folder, const WString& _fileName )
	{
		WString applicationSettingsPath;
		
		applicationSettingsPath += _folder;
		applicationSettingsPath += _fileName;

		printf( "load application config %S ..."
			, applicationSettingsPath.c_str()
			);

		//SI_Error iniError = m_applicationIni.LoadFile( applicationSettingsPath.c_str() );

		SI_Error iniError = s_loadIniFile( m_applicationIni, applicationSettingsPath.c_str() );
		
		if( iniError != 0 )
		{
			m_error = WString( L"Invalid load application settings" ) + applicationSettingsPath;
			return false;
		}

		const WChar * w_game_path = m_applicationIni.GetValue( L"Game", L"Path" );

		if( w_game_path == 0 )
		{
			m_error = WString( L"Not found Game Path" ) + applicationSettingsPath;
			return false;
		}

		const WChar * w_resource_path = m_applicationIni.GetValue( L"Resource", L"Path" );

		if( w_resource_path == 0 )
		{
			m_error = WString( L"Not found Resources Path " ) + applicationSettingsPath;
			return false;
		}

		WString gameIniPath;

		gameIniPath += _folder;
		gameIniPath += w_game_path;

		if( this->loadGame_( gameIniPath ) == false )
		{
			m_error = WString( L"Invalid load game setting" ) + gameIniPath;
			return false;
		}

		WString resourcesIniPath;

		resourcesIniPath += _folder;
		resourcesIniPath += w_resource_path;

		if( this->loadResourcePacks_( resourcesIniPath ) == false )
		{
			m_error = WString( L"Invalid load resourcesPack setting" ) + resourcesIniPath;
			return false;
		}

		return true;
	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::loadGame_( const WString& _path )
	{
		//SI_Error game_error = m_gameIni.LoadFile( _path.c_str() );

		SI_Error game_error = s_loadIniFile( m_gameIni, _path.c_str() );

		if( game_error != 0 )
		{
			return false;
		}

		m_settings.alreadyRunning = false;

		m_settings.applicationSettings.fixedContentResolution = true;
		m_settings.applicationSettings.windowModeCheck = false;
		m_settings.applicationSettings.fullscreen = true;
		m_settings.applicationSettings.vsync = true;

		s_getIniValue( m_gameIni, L"Project", L"Name", m_settings.projectName );
		s_getIniValue( m_gameIni, L"Project", L"Company", m_settings.companyName );
		s_getIniValue( m_gameIni, L"Project", L"Codename", m_settings.applicationSettings.projectCodename );
		s_getIniValue( m_gameIni, L"Locale", L"Default", m_settings.defaultLocale );
		s_getIniValue( m_gameIni, L"Game", L"ContentResolution", m_settings.applicationSettings.contentResolution );
		s_getIniValue( m_gameIni, L"Game", L"LowContentViewport", m_settings.applicationSettings.lowContentViewport );
		s_getIniValue( m_gameIni, L"Game", L"FixedContentResolution", m_settings.applicationSettings.fixedContentResolution );
		s_getIniValue( m_gameIni, L"Game", L"PersonalityModule", m_settings.applicationSettings.personalityModule );
		s_getIniValue( m_gameIni, L"Game", L"AlreadyRunning", m_settings.alreadyRunning );
		s_getIniValue( m_gameIni, L"Game", L"WindowModeCheck", m_settings.applicationSettings.windowModeCheck );
		s_getIniValue( m_gameIni, L"Window", L"Size", m_settings.applicationSettings.windowResolution );
		s_getIniValue( m_gameIni, L"Window", L"Bits", m_settings.applicationSettings.bits );
		s_getIniValue( m_gameIni, L"Window", L"Fullscreen", m_settings.applicationSettings.fullscreen );
		s_getIniValue( m_gameIni, L"Window", L"VSync", m_settings.applicationSettings.vsync );		

		s_getIniAllSettings( m_gameIni, L"Params", m_settings.appParams );

		s_getIniValues( m_gameIni, L"Plugins", L"Name", m_settings.plugins );

		return true;

	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::loadResourcePacks_( const WString & _path )
	{
		//SI_Error error = m_resourcePackIni.LoadFile( _path.c_str() );
		SI_Error error = s_loadIniFile( m_resourcePackIni, _path.c_str() );

		if( error != 0 )
		{
			return false;
		}

		TVectorWString resourcePacksSettings;
		s_getIniValues( m_resourcePackIni, L"GAME_RESOURCES", L"ResourcePack", resourcePacksSettings );

		TVectorResourcePackDesc resourcePacks;

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

			m_settings.resourcePacks.push_back( pack );
		}

		TVectorWString languagePackSettings;
		s_getIniValues( m_resourcePackIni, L"GAME_RESOURCES", L"LanguagePack", languagePackSettings );

		TVectorResourcePackDesc languagePacks;

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

			m_settings.languagePacks.push_back( pack );
		}

		return true;
	}
	/////////////////////////////////////////////////////
	const WChar * StartupConfigLoader::getLastError()
	{
		return m_error.c_str();
	}
	/////////////////////////////////////////////////////
	const StartupSettings& StartupConfigLoader::getSettings()
	{
		return m_settings;
	}
	/////////////////////////////////////////////////////

}