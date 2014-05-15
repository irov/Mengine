#	pragma once

#	include "Core/String.h"
#   include "Core/FilePath.h"

#   include "Interface/StreamInterface.h"
#   include "Interface/ApplicationInterface.h"

namespace Menge
{   
    class ServiceProviderInterface;

	struct StartupSettings
	{
		StartupSettings()
			: maxfps(false)
			, alreadyRunning(false)
		{
		}

		ApplicationSettings applicationSettings;

        WString personalityModule;
		
		TMapParams appParams;
		TVectorWString plugins;		
        TVectorWString devPlugins;

		TVectorString modules;
		
		WString companyName;
		WString projectName;		
		String defaultLocale;
		
		TVectorResourcePackDesc resourcePacks;
		TVectorResourcePackDesc languagePacks;

        bool maxfps;
        bool alreadyRunning;
	};

	class StartupConfigLoader
	{
	public:
		StartupConfigLoader( ServiceProviderInterface * _serviceProvider );
		~StartupConfigLoader();

	public:
		bool load( const ConstString & _fileGroup, const FilePath & _applicationPath, StartupSettings & _settings );

    protected:
		bool loadGame_( const ConstString & _fileGroup, const FilePath & _gameIni, StartupSettings & _settings );
		bool loadResourcePacks_( const ConstString & _fileGroup, const FilePath & _resourceIni, StartupSettings & _settings );

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}
