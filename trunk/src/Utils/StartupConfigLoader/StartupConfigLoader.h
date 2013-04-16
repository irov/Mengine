#	pragma once

#	include "SimpleIni.h"

#	include "Core/String.h"
#   include "Core/FilePath.h"

#   include "Interface/ApplicationInterface.h"

namespace Menge
{   
    class ServiceProviderInterface;

    class InputStreamInterface;

	struct StartupSettings
	{
		ApplicationSettings applicationSettings;

        WString personalityModule;
		
		TMapWString appParams;
		TVectorWString plugins;
        TVectorWString devPlugins;
		
		WString projectName;
		WString companyName;
		String defaultLocale;		
		
		TVectorResourcePackDesc resourcePacks;
		TVectorResourcePackDesc languagePacks;

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
		bool loadGame_( InputStreamInterface * _inputStream, StartupSettings & _settings );
		bool loadResourcePacks_( InputStreamInterface * _inputStream, StartupSettings & _settings );

    protected:
        ServiceProviderInterface * m_serviceProvider;

		CSimpleIniCaseW m_applicationIni;
		CSimpleIniCaseW m_gameIni;
		CSimpleIniCaseW m_resourcePackIni;
	};
}
