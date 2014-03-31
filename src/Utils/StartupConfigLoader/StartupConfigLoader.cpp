#	include "StartupConfigLoader.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Core/IniUtil.h"

#   include "Logger/Logger.h"

namespace Menge
{
	/////////////////////////////////////////////////////
	StartupConfigLoader::StartupConfigLoader( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)		
	{
	}
	/////////////////////////////////////////////////////
	StartupConfigLoader::~StartupConfigLoader()
	{
	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::load( const ConstString & _fileGroup, const FilePath & _applicationPath, StartupSettings & _settings )
	{
        InputStreamInterfacePtr applicationInputStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, _applicationPath );

        if( applicationInputStream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid open application settings %s"
                , _applicationPath.c_str()
                );

            return false;
        }

		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, applicationInputStream, m_serviceProvider ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid load application settings %s"
                , _applicationPath.c_str()
                );

            return false;
        }

        applicationInputStream = nullptr;
		
        const char * gameIniPath = ini.getSettingValue( "Game", "Path" );
        		
		if( gameIniPath == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Not found Game Path %s"
                , _applicationPath.c_str()
                );

			return false;
		}
       
        FilePath c_gameIniPath = Helper::stringizeString( m_serviceProvider, gameIniPath );

        InputStreamInterfacePtr gameInputStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, c_gameIniPath );

        if( gameInputStream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid open game setting %s"
                , c_gameIniPath.c_str()
                );

            return false;
        }
        
		if( this->loadGame_( _fileGroup, c_gameIniPath, _settings ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid load game setting %s"
                , c_gameIniPath.c_str()
                );
            			
			return false;
		}

        const char * resourcesIniPath = ini.getSettingValue( "Resource", "Path" );

        if( resourcesIniPath == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Not found Resources Path %s"
                , _applicationPath.c_str()
                );

            return false;
        }

        FilePath c_resourcesIniPath = Helper::stringizeString( m_serviceProvider, resourcesIniPath );

		if( this->loadResourcePacks_( _fileGroup, c_resourcesIniPath, _settings ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid load resourcesPack setting %s"
                , c_resourcesIniPath.c_str()
                );

			return false;
		}               
        
		return true;
	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::loadGame_( const ConstString & _fileGroup, const FilePath & _gameIniPath, StartupSettings & _settings )
	{
        InputStreamInterfacePtr gameInputStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, _gameIniPath );

        if( gameInputStream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid open game setting '%s'"
                , _gameIniPath.c_str()
                );

            return false;
        }


		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, gameInputStream, m_serviceProvider ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::loadGame_ Invalid load game settings '%s'"
                , _gameIniPath.c_str()
                );

            return false;
        }

        gameInputStream = nullptr;

		IniUtil::getIniValue( ini, "Project", "Name", _settings.projectName, m_serviceProvider );
		IniUtil::getIniValue( ini, "Project", "Company", _settings.companyName, m_serviceProvider );
		IniUtil::getIniValue( ini, "Project", "Codename", _settings.applicationSettings.projectCodename, m_serviceProvider );
		IniUtil::getIniValue( ini, "Project", "Version", _settings.applicationSettings.projectVersion, m_serviceProvider );
		IniUtil::getIniValue( ini, "Locale", "Default", _settings.defaultLocale, m_serviceProvider );
		IniUtil::getIniValue( ini, "Game", "ContentResolution", _settings.applicationSettings.contentResolution, m_serviceProvider );     
		IniUtil::getIniValue( ini, "Game", "FixedContentResolution", _settings.applicationSettings.fixedContentResolution, m_serviceProvider );
		IniUtil::getIniValue( ini, "Game", "PersonalityModule", _settings.personalityModule, m_serviceProvider );
		IniUtil::getIniValue( ini, "Game", "AlreadyRunning", _settings.alreadyRunning, m_serviceProvider );
		IniUtil::getIniValue( ini, "Game", "WindowModeCheck", _settings.applicationSettings.windowModeCheck, m_serviceProvider );
		IniUtil::getIniValue( ini, "Window", "Size", _settings.applicationSettings.windowResolution, m_serviceProvider );
		IniUtil::getIniValue( ini, "Window", "Bits", _settings.applicationSettings.bits, m_serviceProvider );
		IniUtil::getIniValue( ini, "Window", "Fullscreen", _settings.applicationSettings.fullscreen, m_serviceProvider );
        IniUtil::getIniValue( ini, "Window", "MaxFPS", _settings.maxfps, m_serviceProvider );
		IniUtil::getIniValue( ini, "Window", "VSync", _settings.applicationSettings.vsync, m_serviceProvider );

        IniUtil::getIniValue( ini, "Game", "AspectRatioViewport", _settings.applicationSettings.aspectRatioViewports, m_serviceProvider );

		IniUtil::getIniAllSettings( ini, "Params", _settings.appParams, m_serviceProvider );

		IniUtil::getIniValue( ini, "Plugins", "Name", _settings.plugins, m_serviceProvider );
		IniUtil::getIniValue( ini, "Modules", "Name", _settings.modules, m_serviceProvider );
        IniUtil::getIniValue( ini, "DevPlugins", "Name", _settings.devPlugins, m_serviceProvider );
        
		return true;
	}
	/////////////////////////////////////////////////////
	bool StartupConfigLoader::loadResourcePacks_( const ConstString & _fileGroup, const FilePath & _resourceIni, StartupSettings & _settings )
	{
        InputStreamInterfacePtr resourceInputStream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, _resourceIni );

        if( resourceInputStream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::load Invalid open resourcesPack setting '%s'"
                , _resourceIni.c_str()
                );

            return false;
        }

		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, resourceInputStream, m_serviceProvider ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("StartupConfigLoader::loadResourcePacks_ Invalid load resource settings '%s'"
                , _resourceIni.c_str()
                );

            return false;
        }

        resourceInputStream = nullptr;

		TVectorString resourcePacksSettings;
		IniUtil::getIniValue( ini, "GAME_RESOURCES", "ResourcePack", resourcePacksSettings, m_serviceProvider );

        _settings.resourcePacks.reserve( resourcePacksSettings.size() );
        
        ConstString c_dir = Helper::stringizeStringSize(m_serviceProvider, "dir", 3);

		for( TVectorString::iterator
			it = resourcePacksSettings.begin(),
			it_end = resourcePacksSettings.end();
		it != it_end;
		++it )
		{
			const String & resourcePack = *it;

            ResourcePackDesc pack;

            pack.dev = false;
			pack.preload = true;
			pack.type = c_dir;

			IniUtil::getIniValue( ini, resourcePack.c_str(), "Name", pack.name, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Type", pack.type, m_serviceProvider );            
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Path", pack.path, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Locale", pack.locale, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Platform", pack.platform, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Description", pack.descriptionPath, m_serviceProvider );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Dev", pack.dev, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "PreLoad", pack.preload, m_serviceProvider );			

            _settings.resourcePacks.push_back( pack );
		}

		TVectorString languagePackSettings;
		IniUtil::getIniValue( ini, "GAME_RESOURCES", "LanguagePack", languagePackSettings, m_serviceProvider );

        _settings.languagePacks.reserve( languagePackSettings.size() );

		for( TVectorString::iterator
			it = languagePackSettings.begin(),
			it_end = languagePackSettings.end();
		it != it_end;
		++it )
		{
			const String & resourcePack = *it;

            ResourcePackDesc pack;
            
            pack.dev = false;
			pack.preload = true;
			pack.type = c_dir;

			IniUtil::getIniValue( ini, resourcePack.c_str(), "Name", pack.name, m_serviceProvider );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Type", pack.type, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Path", pack.path, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Locale", pack.locale, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Platform", pack.platform, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Description", pack.descriptionPath, m_serviceProvider );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Dev", pack.dev, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "PreLoad", pack.preload, m_serviceProvider );	

			//IniUtil::getIniValue( ini, resourcePack.c_str(), "Text", pack.texts, m_serviceProvider );
			//IniUtil::getIniValue( ini, resourcePack.c_str(), "Font", pack.fonts, m_serviceProvider );
			
			_settings.languagePacks.push_back( pack );
		}

		return true;
	}
	/////////////////////////////////////////////////////

}