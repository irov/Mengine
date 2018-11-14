#include "PackageService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Logger.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Kernel/IniUtil.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PackageService, Mengine::PackageService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PackageService::PackageService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PackageService::~PackageService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::_initializeService()
    {
        NOTIFICATION_SERVICE()
            ->addObserverMethod( NOTIFICATOR_CHANGE_LOCALE, this, &PackageService::notifyChangeLocale );

        m_factoryPackage = new FactoryPool<Package, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PackageService::_finalizeService()
    {
        NOTIFICATION_SERVICE()
            ->removeObserver( NOTIFICATOR_CHANGE_LOCALE, this );

        m_packages.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPackage );

        m_factoryPackage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::loadPackages( const FileGroupInterfacePtr & _fileGroup, const FilePath & _resourceIni )
    {
        LOGGER_WARNING( "Packages load..."
        );

        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, _fileGroup, _resourceIni ) == false )
        {
            LOGGER_ERROR( "PackageService::loadPackages Invalid load resource settings '%s'"
                , _resourceIni.c_str()
            );

            return false;
        }

        ConstString c_dir = STRINGIZE_STRING_LOCAL( "dir" );

        VectorString frameworkPacksSettings;
        IniUtil::getIniValue( ini, "GAME_RESOURCES", "FrameworkPack", frameworkPacksSettings );

        for( const String & resourcePack : frameworkPacksSettings )
        {
            LOGGER_WARNING( "Package '%s'"
                , resourcePack.c_str()
            );

            PackageDesc pack;

            pack.dev = false;
            pack.immediately = true;
            pack.preload = true;
            pack.type = c_dir;

            if( ini.hasSection( resourcePack.c_str() ) == false )
            {
                LOGGER_CRITICAL( "PackageService::loadPackages %s invalid load resource pack no found section for '%s'"
                    , _resourceIni.c_str()
                    , resourcePack.c_str()
                );

                return false;
            }

            IniUtil::getIniValue( ini, resourcePack.c_str(), "Name", pack.name );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Type", pack.type );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Category", pack.category );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Path", pack.path );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Locale", pack.locale );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Platform", pack.platform );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Description", pack.descriptionPath );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Dev", pack.dev );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "PreLoad", pack.preload );

            if( this->addPackage( pack ) == false )
            {
                return false;
            }
        }

        VectorString resourcePacksSettings;
        IniUtil::getIniValue( ini, "GAME_RESOURCES", "ResourcePack", resourcePacksSettings );

        for( const String & resourcePack : resourcePacksSettings )
        {
            LOGGER_WARNING( "Package '%s'"
                , resourcePack.c_str()
            );

            PackageDesc pack;

            pack.dev = false;
            pack.immediately = false;
            pack.preload = true;
            pack.type = c_dir;

            if( ini.hasSection( resourcePack.c_str() ) == false )
            {
                LOGGER_CRITICAL( "PackageService::loadPackages %s invalid load resource pack no found section for '%s'"
                    , _resourceIni.c_str()
                    , resourcePack.c_str()
                );

                return false;
            }

            IniUtil::getIniValue( ini, resourcePack.c_str(), "Name", pack.name );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Type", pack.type );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Category", pack.category );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Path", pack.path );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Locale", pack.locale );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Platform", pack.platform );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Description", pack.descriptionPath );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "Dev", pack.dev );
            IniUtil::getIniValue( ini, resourcePack.c_str(), "PreLoad", pack.preload );

            if( this->addPackage( pack ) == false )
            {
                return false;
            }
        }

        VectorString languagePackSettings;
        IniUtil::getIniValue( ini, "GAME_RESOURCES", "LanguagePack", languagePackSettings );

        for( const String & languagePack : languagePackSettings )
        {
            LOGGER_WARNING( "Package '%s'"
                , languagePack.c_str()
            );

            PackageDesc pack;

            pack.dev = false;
            pack.immediately = false;
            pack.preload = true;
            pack.type = c_dir;

            if( ini.hasSection( languagePack.c_str() ) == false )
            {
                LOGGER_CRITICAL( "PackageService::loadPackages %s invalid load language pack no found section for '%s'"
                    , _resourceIni.c_str()
                    , languagePack.c_str()
                );

                return false;
            }

            IniUtil::getIniValue( ini, languagePack.c_str(), "Name", pack.name );
            IniUtil::getIniValue( ini, languagePack.c_str(), "Type", pack.type );
            IniUtil::getIniValue( ini, languagePack.c_str(), "Category", pack.category );
            IniUtil::getIniValue( ini, languagePack.c_str(), "Path", pack.path );
            IniUtil::getIniValue( ini, languagePack.c_str(), "Locale", pack.locale );
            IniUtil::getIniValue( ini, languagePack.c_str(), "Platform", pack.platform );
            IniUtil::getIniValue( ini, languagePack.c_str(), "Description", pack.descriptionPath );
            IniUtil::getIniValue( ini, languagePack.c_str(), "Dev", pack.dev );
            IniUtil::getIniValue( ini, languagePack.c_str(), "PreLoad", pack.preload );

            if( this->addPackage( pack ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::hasPackage( const ConstString & _name ) const
    {
        for( const PackagePtr & package : m_packages )
        {
            const ConstString & packName = package->getName();

            if( packName != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::addPackage( const PackageDesc & _desc )
    {
        bool developmentMode = HAS_OPTION( "dev" );

        if( developmentMode == false && _desc.dev == true )
        {
            return true;
        }

        FileGroupInterfacePtr category;
        if( FILE_SERVICE()
            ->hasFileGroup( _desc.category, &category ) == false )
        {
            LOGGER_ERROR( "PackageService::addPackage invalid found file group %s"
                , _desc.category.c_str()
            );

            return false;
        }

        PackagePtr package = m_factoryPackage->createObject();

        package->setup( _desc.name
            , _desc.type
            , _desc.locale
            , _desc.platform
            , _desc.descriptionPath
            , category
            , _desc.path
            , _desc.preload
        );

        if( package->load() == false )
        {
            LOGGER_ERROR( "PackageService::addPackage invalid load package '%s' path '%s'"
                , package->getName().c_str()
                , package->getPath().c_str()
            );

            return false;
        }

        m_packages.emplace_back( package );

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
    bool PackageService::removePackage( const ConstString & _name )
    {
        for( VectorPackages::const_iterator
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

            package->disable();

            m_packages.erase( it );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    PackageInterfacePtr PackageService::getPackage( const ConstString & _name ) const
    {
        for( const PackagePtr & package : m_packages )
        {
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
    bool PackageService::loadLocalePacksByName_( VectorPackages & _packs, const ConstString & _locale, const Tags & _platformTags ) const
    {
        bool hasLocale = false;

        for( const PackagePtr & package : m_packages )
        {
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

            _packs.emplace_back( package );

            hasLocale = true;
        }

        return hasLocale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::enablePackages( const ConstString & _locale, const Tags & _platformTags )
    {
        LOGGER_WARNING( "Packages enable..."
        );

        VectorPackages packages;

        for( const PackagePtr & pack : m_packages )
        {
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

            packages.emplace_back( pack );
        }

        if( this->loadLocalePacksByName_( packages, _locale, _platformTags ) == false )
        {
            if( this->loadLocalePacksByName_( packages, STRINGIZE_STRING_LOCAL( "en" ), _platformTags ) == false )
            {
                LOGGER_WARNING( "PackageService::applyPackages not set locale pack"
                );
            }
        }

        for( const PackagePtr & package : packages )
        {
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

        if( NOTIFICATION_SERVICE()
            ->notify( NOTIFICATOR_ENGINE_ENABLE_PACKAGES ) == false )
        {
            LOGGER_ERROR( "Resources validation is invalid!!!!!!!!!!!!!" );

            bool resourceCheckCritical = HAS_OPTION( "noresourcecheckcritical" );

            if( resourceCheckCritical == false )
            {
                LOGGER_CRITICAL( "Fix Resources" );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::enableLocalePackage( const ConstString & _locale, const Tags & _platformTag )
    {
        VectorPackages packages;

        for( const PackagePtr & package : m_packages )
        {
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

            packages.emplace_back( package );
        }

        for( const PackagePtr & package : packages )
        {
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
        VectorPackages packages;

        for( const PackagePtr & package : m_packages )
        {
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

            packages.emplace_back( package );
        }

        for( const PackagePtr & package : packages )
        {
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

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        if( this->disableLocalePackage( _prevLocale, platformTags ) == false )
        {
            LOGGER_ERROR( "PackageService::notifyChangeLocale invalid disable locale package '%s' platform '%s'"
                , _prevLocale.c_str()
                , platformTags.to_str().c_str()
            );

            return;
        }

        if( this->enableLocalePackage( _currentlocale, platformTags ) == false )
        {
            LOGGER_ERROR( "PackageService::notifyChangeLocale invalid enable locale package '%s' platform '%s'"
                , _currentlocale.c_str()
                , platformTags.to_str().c_str()
            );

            return;
        }
    }
}