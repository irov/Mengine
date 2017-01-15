#   include "PackageService.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/OptionsInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#	include "Core/IniUtil.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PackageService, Menge::PackageService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	PackageService::PackageService()
		: m_observerChangeLocale(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
	PackageService::~PackageService()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool PackageService::_initialize()
	{
		m_observerChangeLocale = NOTIFICATION_SERVICE( m_serviceProvider )
			->addObserverMethod( NOTIFICATOR_CHANGE_LOCALE, this, &PackageService::notifyChangeLocale );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PackageService::_finalize()
	{
		m_observerChangeLocale = nullptr;

		m_packages.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool PackageService::loadPackages( const ConstString & _fileGroup, const FilePath & _resourceIni )
	{ 
		InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
			->openInputFile( _fileGroup, _resourceIni, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("PackageService::loadPackages Invalid open resourcesPack setting '%s'"
				, _resourceIni.c_str()
				);

			return false;
		}

		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, stream, m_serviceProvider ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PackageService::loadPackages Invalid load resource settings '%s'"
				, _resourceIni.c_str()
				);

			return false;
		}

		stream = nullptr;

		ConstString c_dir = STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" );

		TVectorString frameworkPacksSettings;
		IniUtil::getIniValue( ini, "GAME_RESOURCES", "FrameworkPack", frameworkPacksSettings, m_serviceProvider );

		for( TVectorString::iterator
			it = frameworkPacksSettings.begin(),
			it_end = frameworkPacksSettings.end();
		it != it_end;
		++it )
		{
			const String & resourcePack = *it;

			PackageDesc desc;

			desc.dev = false;
			desc.immediately = true;
			desc.preload = true;
			desc.type = c_dir;

			if( ini.hasSection( resourcePack.c_str() ) == false )
			{
				LOGGER_CRITICAL( m_serviceProvider )("PackageService::loadPackages %s invalid load resource pack no found section for '%s'"
					, _resourceIni.c_str()
					, resourcePack.c_str()
					);

				return false;
			}

			IniUtil::getIniValue( ini, resourcePack.c_str(), "Name", desc.name, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Type", desc.type, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Path", desc.path, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Locale", desc.locale, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Platform", desc.platform, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Description", desc.descriptionPath, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Dev", desc.dev, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "PreLoad", desc.preload, m_serviceProvider );

			if( this->addPackage( desc ) == false )
			{
				return false;
			}
		}

		TVectorString resourcePacksSettings;
		IniUtil::getIniValue( ini, "GAME_RESOURCES", "ResourcePack", resourcePacksSettings, m_serviceProvider );

		for( TVectorString::iterator
			it = resourcePacksSettings.begin(),
			it_end = resourcePacksSettings.end();
		it != it_end;
		++it )
		{
			const String & resourcePack = *it;

			PackageDesc pack;

			pack.dev = false;
			pack.immediately = false;
			pack.preload = true;
			pack.type = c_dir;

			if( ini.hasSection( resourcePack.c_str() ) == false )
			{
				LOGGER_CRITICAL( m_serviceProvider )("PackageService::loadPackages %s invalid load resource pack no found section for '%s'"
					, _resourceIni.c_str()
					, resourcePack.c_str()
					);

				return false;
			}

			IniUtil::getIniValue( ini, resourcePack.c_str(), "Name", pack.name, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Type", pack.type, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Path", pack.path, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Locale", pack.locale, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Platform", pack.platform, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Description", pack.descriptionPath, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Dev", pack.dev, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "PreLoad", pack.preload, m_serviceProvider );

			if( this->addPackage( pack ) == false )
			{
				return false;
			}
		}

		TVectorString languagePackSettings;
		IniUtil::getIniValue( ini, "GAME_RESOURCES", "LanguagePack", languagePackSettings, m_serviceProvider );

		for( TVectorString::iterator
			it = languagePackSettings.begin(),
			it_end = languagePackSettings.end();
		it != it_end;
		++it )
		{
			const String & languagePack = *it;

			PackageDesc pack;

			pack.dev = false;
			pack.immediately = false;
			pack.preload = true;
			pack.type = c_dir;

			if( ini.hasSection( languagePack.c_str() ) == false )
			{
				LOGGER_CRITICAL( m_serviceProvider )("PackageService::loadPackages %s invalid load language pack no found section for '%s'"
					, _resourceIni.c_str()
					, languagePack.c_str()
					);

				return false;
			}

			IniUtil::getIniValue( ini, languagePack.c_str(), "Name", pack.name, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Type", pack.type, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Path", pack.path, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Locale", pack.locale, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Platform", pack.platform, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Description", pack.descriptionPath, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Dev", pack.dev, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "PreLoad", pack.preload, m_serviceProvider );

			if( this->addPackage( pack ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PackageService::addPackage( const PackageDesc & _desc )
	{
		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );

		if( developmentMode == false && _desc.dev == true )
		{
			return true;
		}

		PackagePtr package = m_factoryPackage.createObject();

        package->setServiceProvider( m_serviceProvider );

		package->setup( _desc.name
			, _desc.type
			, _desc.locale
			, _desc.platform
			, _desc.descriptionPath
			, _desc.path
			, _desc.preload
			);

		if( package->load() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PackageService::addPackage invalid load package '%s' path '%s'"
				, package->getName().c_str()
				, package->getPath().c_str()
				);

			return false;
		}
				
		m_packages.push_back( package );
		
		if( _desc.immediately == true )
		{
			if( package->enable() == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PackageInterfacePtr PackageService::getPackage( const ConstString & _name ) const
	{
		for( TVectorPackage::const_iterator
			it = m_packages.begin(),
			it_end = m_packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & package = *it;

			const ConstString & packName = package->getName();

			if( packName != _name )
			{
				continue;
			}

			return package;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PackageService::loadLocalePacksByName_( TVectorPackage & _packs, const ConstString & _locale, const Tags & _platformTags ) const
	{
		bool hasLocale = false;

		for( TVectorPackage::const_iterator
			it = m_packages.begin(),
			it_end = m_packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & package = *it;

			const ConstString & packageLocale = package->getLocale();

			if( packageLocale != _locale )
			{
				continue;
			}

			const Tags & packagePlatform = package->getPlatfromTags();

			if( _platformTags.inTags( packagePlatform ) == false )
			{
				continue;
			}

			_packs.push_back( package );

			hasLocale = true;
		}

		return hasLocale;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PackageService::enablePackages( const ConstString & _locale, const Tags & _platformTags )
	{
		TVectorPackage packages;

		for( TVectorPackage::const_iterator
			it = m_packages.begin(),
			it_end = m_packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & pack = *it;

			const Tags & packPlatform = pack->getPlatfromTags();

			if( _platformTags.inTags( packPlatform ) == false )
			{
				continue;
			}

			const ConstString & locale = pack->getLocale();

			if( locale.empty() == false )
			{
				continue;
			}

			packages.push_back( pack );
		}

		if( this->loadLocalePacksByName_( packages, _locale, _platformTags ) == false )
		{
			if( this->loadLocalePacksByName_( packages, STRINGIZE_STRING_LOCAL( m_serviceProvider, "en" ), _platformTags ) == false )
			{
				LOGGER_WARNING( m_serviceProvider )("PackageService::applyPackages not set locale pack"
					);
			}
		}

		for( TVectorPackage::const_iterator
			it = packages.begin(),
			it_end = packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & package = *it;

			if( package->isPreload() == false )
			{
				continue;
			}

			if( package->isEnable() == true )
			{
				continue;
			}

			if( package->enable() == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PackageService::validatePackages() const
	{
		bool successful = true;

		for( TVectorPackage::const_iterator
			it = m_packages.begin(),
			it_end = m_packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & package = *it;

			if( package->validate() == false )
			{
				successful = false;

				continue;
			}
		}

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PackageService::enableLocalePackage( const ConstString & _locale, const Tags & _platformTag )
	{ 
		TVectorPackage packages;

		for( TVectorPackage::const_iterator
			it = m_packages.begin(),
			it_end = m_packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & package = *it;

			const Tags & platformTags = package->getPlatfromTags();

			if( _platformTag.inTags( platformTags ) == false )
			{
				continue;
			}

			const ConstString & locale = package->getLocale();

			if( locale != _locale )
			{
				continue;
			}

			packages.push_back( package );
		}

		for( TVectorPackage::const_iterator
			it = packages.begin(),
			it_end = packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & package = *it;

			if( package->enable() == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PackageService::disableLocalePackage( const ConstString & _locale, const Tags & _platformTag )
	{
		TVectorPackage packages;

		for( TVectorPackage::const_iterator
			it = m_packages.begin(),
			it_end = m_packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & package = *it;

			const Tags & platformTags = package->getPlatfromTags();

			if( _platformTag.inTags( platformTags ) == false )
			{
				continue;
			}

			const ConstString & locale = package->getLocale();

			if( locale != _locale )
			{
				continue;
			}

			packages.push_back( package );
		}

		for( TVectorPackage::const_iterator
			it = packages.begin(),
			it_end = packages.end();
		it != it_end;
		++it )
		{
			const PackagePtr & package = *it;

			if( package->disable() == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PackageService::notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale )
	{
		(void)_prevLocale;
		(void)_currentlocale;

		const Tags & platformTags = PLATFORM_SERVICE( m_serviceProvider )
			->getPlatformTags();

		if( this->disableLocalePackage( _prevLocale, platformTags ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PackageService::notifyChangeLocale invalid disable locale package '%s' platform '%s'"
				, _prevLocale.c_str()
				, platformTags.to_str().c_str()
				);

			return;
		}

		if( this->enableLocalePackage( _currentlocale, platformTags ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("PackageService::notifyChangeLocale invalid enable locale package '%s' platform '%s'"
				, _currentlocale.c_str()
				, platformTags.to_str().c_str()
				);

			return;
		}
	}
}