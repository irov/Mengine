#	pragma once

#	include "SimpleIni.h"

#	include "Core/String.h"
#	include "Interface/ApplicationInterface.h"

#	include "stdio.h"

//#define STARTUP_LOADER_ERROR_LENGTH 255

namespace Menge
{
	struct StartupSettings
	{
		ApplicationSettings applicationSettings;
		
		TMapWString appParams;
		TVectorWString plugins;
		
		WString projectName;
		WString companyName;
		String defaultLocale;
		bool alreadyRunning;
		
		TVectorResourcePackDesc resourcePacks;
		TVectorResourcePackDesc languagePacks;
	};

	class StartupConfigLoader
	{
	public:
		StartupConfigLoader();
		~StartupConfigLoader();

	public:
		bool load( const WString& _folder, const WString& _fileName );

		const wchar_t * getLastError();

		const StartupSettings& getSettings();

	private:
		bool loadGame_( const WString& _path );
		bool loadResourcePacks_(  const WString& _path );

		CSimpleIniCaseW m_applicationIni;
		CSimpleIniCaseW m_gameIni;
		CSimpleIniCaseW m_resourcePackIni;
		StartupSettings m_settings;
		wchar_t m_error[255];
	};
}
