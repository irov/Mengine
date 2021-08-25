#include "PackageService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/TagsHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/BuildMode.h"

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
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_LOCALE, &PackageService::notifyChangeLocale, MENGINE_DOCUMENT_FACTORABLE );

        m_factoryPackage = Helper::makeFactoryPool<Package, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PackageService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE );

        for( const PackagePtr & package : m_packages )
        {
            package->finalize();
        }

        m_packages.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPackage );

        m_factoryPackage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::loadPackages( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "package", "Packages load... %s:%s"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
        );

        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _fileGroup, _filePath, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load packages '%s'"
            , _filePath.c_str()
        );

        VectorString frameworkPacksSettings;
        config->getValues( "GAME_PACKAGES", "FrameworkPack", &frameworkPacksSettings );

        for( const String & frameworkPack : frameworkPacksSettings )
        {
            LOGGER_MESSAGE( "Package '%s'"
                , frameworkPack.c_str()
            );

            PackageDesc pack;

            pack.dev = false;
            pack.immediately = true;
            pack.preload = true;
            pack.type = STRINGIZE_STRING_LOCAL( "dir" );

            if( config->hasSection( frameworkPack.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' framework package no found section for '%s'"
                    , _filePath.c_str()
                    , frameworkPack.c_str()
                );

                return false;
            }

            config->hasValue( frameworkPack.c_str(), "Name", &pack.name );
            config->hasValue( frameworkPack.c_str(), "Type", &pack.type );
            config->hasValue( frameworkPack.c_str(), "Locale", &pack.locale );
            config->hasValue( frameworkPack.c_str(), "Platform", &pack.platform );
            config->hasValue( frameworkPack.c_str(), "Tag", &pack.tags );
            config->hasValue( frameworkPack.c_str(), "Category", &pack.fileGroupName );
            config->hasValue( frameworkPack.c_str(), "Parent", &pack.parent );
            config->hasValue( frameworkPack.c_str(), "Description", &pack.descriptionPath );
            config->hasValue( frameworkPack.c_str(), "Path", &pack.path );
            config->hasValue( frameworkPack.c_str(), "Dev", &pack.dev );
            config->hasValue( frameworkPack.c_str(), "PreLoad", &pack.preload );

            if( this->addPackage( pack, MENGINE_DOCUMENT_MESSAGE( "framework '%s'", frameworkPack.c_str() ) ) == false )
            {
                LOGGER_ERROR( "invalid add framework package '%s'"
                    , pack.name.c_str()
                );

                return false;
            }
        }

        VectorString resourcePacksSettings;
        config->getValues( "GAME_PACKAGES", "ResourcePack", &resourcePacksSettings );

        for( const String & resourcePack : resourcePacksSettings )
        {
            LOGGER_MESSAGE( "Package '%s'"
                , resourcePack.c_str()
            );

            PackageDesc pack;

            pack.dev = false;
            pack.immediately = false;
            pack.preload = true;
            pack.type = STRINGIZE_STRING_LOCAL( "dir" );

            if( config->hasSection( resourcePack.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' resource package no found section for '%s'"
                    , _filePath.c_str()
                    , resourcePack.c_str()
                );

                return false;
            }

            config->hasValue( resourcePack.c_str(), "Name", &pack.name );
            config->hasValue( resourcePack.c_str(), "Type", &pack.type );
            config->hasValue( resourcePack.c_str(), "Locale", &pack.locale );
            config->hasValue( resourcePack.c_str(), "Platform", &pack.platform );
            config->hasValue( resourcePack.c_str(), "Tag", &pack.tags );
            config->hasValue( resourcePack.c_str(), "Category", &pack.fileGroupName );
            config->hasValue( resourcePack.c_str(), "FileGroup", &pack.fileGroupName );
            config->hasValue( resourcePack.c_str(), "Parent", &pack.parent );
            config->hasValue( resourcePack.c_str(), "Description", &pack.descriptionPath );
            config->hasValue( resourcePack.c_str(), "Path", &pack.path );
            config->hasValue( resourcePack.c_str(), "Dev", &pack.dev );
            config->hasValue( resourcePack.c_str(), "PreLoad", &pack.preload );

            if( this->addPackage( pack, MENGINE_DOCUMENT_MESSAGE( "framework '%s'", resourcePack.c_str() ) ) == false )
            {
                LOGGER_ERROR( "invalid add resource package '%s'"
                    , pack.name.c_str()
                );

                return false;
            }
        }

        VectorString languagePackSettings;
        config->getValues( "GAME_PACKAGES", "LanguagePack", &languagePackSettings );

        for( const String & languagePack : languagePackSettings )
        {
            LOGGER_MESSAGE( "Package '%s'"
                , languagePack.c_str()
            );

            PackageDesc pack;

            pack.dev = false;
            pack.immediately = false;
            pack.preload = true;
            pack.type = STRINGIZE_STRING_LOCAL( "dir" );

            if( config->hasSection( languagePack.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' language package no found section for '%s'"
                    , _filePath.c_str()
                    , languagePack.c_str()
                );

                return false;
            }

            config->hasValue( languagePack.c_str(), "Name", &pack.name );
            config->hasValue( languagePack.c_str(), "Type", &pack.type );
            config->hasValue( languagePack.c_str(), "Locale", &pack.locale );
            config->hasValue( languagePack.c_str(), "Platform", &pack.platform );
            config->hasValue( languagePack.c_str(), "Tag", &pack.tags );
            config->hasValue( languagePack.c_str(), "Category", &pack.fileGroupName );
            config->hasValue( languagePack.c_str(), "FileGroup", &pack.fileGroupName );
            config->hasValue( languagePack.c_str(), "Parent", &pack.parent );
            config->hasValue( languagePack.c_str(), "Description", &pack.descriptionPath );
            config->hasValue( languagePack.c_str(), "Path", &pack.path );
            config->hasValue( languagePack.c_str(), "Dev", &pack.dev );
            config->hasValue( languagePack.c_str(), "PreLoad", &pack.preload );
            config->hasValue( languagePack.c_str(), "Fonts", &pack.fontsPath );
            config->hasValue( languagePack.c_str(), "Texts", &pack.textsPath );

            if( this->addPackage( pack, MENGINE_DOCUMENT_MESSAGE( "framework '%s'", languagePack.c_str() ) ) == false )
            {
                LOGGER_ERROR( "invalid add language package '%s'"
                    , pack.name.c_str()
                );

                return false;
            }
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_PACKAGES_LOAD );

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
    bool PackageService::addPackage( const PackageDesc & _desc, const DocumentPtr & _doc )
    {
        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == false && _desc.dev == true )
        {
            return true;
        }

        FileGroupInterfacePtr baseFileGroup;
        if( FILE_SERVICE()
            ->hasFileGroup( _desc.fileGroupName, &baseFileGroup ) == false )
        {
            LOGGER_ERROR( "invalid found file group '%s'"
                , _desc.fileGroupName.c_str()
            );

            return false;
        }

        PackagePtr package = m_factoryPackage->createObject( _doc );

        if( package->initialize( _desc.name
            , _desc.type
            , _desc.locale
            , _desc.platform
            , _desc.tags
            , _desc.descriptionPath
            , baseFileGroup
            , _desc.path
            , _desc.fontsPath
            , _desc.textsPath
            , _desc.preload ) == false )
        {
            LOGGER_ERROR( "invalid initialize package '%s' path '%s'"
                , _desc.name.c_str()
                , _desc.path.c_str()
            );

            return false;
        }

        if( _desc.parent.empty() == false )
        {
            const PackageInterfacePtr & parent_package = this->getPackage( _desc.parent );

            package->setParent( parent_package );
        }

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        const Tags & packageTags = package->getPlatfromTags();
        
        if( packageTags.empty() == true || platformTags.hasTags( packageTags ) )
        {
            if( package->load( _doc ) == false )
            {
                LOGGER_ERROR( "invalid load package '%s' path '%s'"
                    , package->getName().c_str()
                    , package->getPathPath().c_str()
                );

                package->finalize();

                return false;
            }
        }

        m_packages.emplace_back( package );

        if( _desc.immediately == true )
        {
            if( packageTags.empty() == true || platformTags.hasTags( packageTags ) )
            {            
                if( package->enable() == false )
                {
                    return false;
                }
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

            package->finalize();

            m_packages.erase( it );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const PackageInterfacePtr & PackageService::getPackage( const ConstString & _name ) const
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

        return PackageInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::existLocalePackage( const ConstString & _locale, const Tags & _platformTags ) const
    {
        for( const PackagePtr & package : m_packages )
        {
            const Tags & packPlatform = package->getPlatfromTags();

            if( _platformTags.hasTags( packPlatform ) == false )
            {
                continue;
            }

            if( _locale.empty() == true )
            {
                return true;
            }

            const ConstString & packageLocale = package->getLocale();

            if( packageLocale.empty() == true )
            {
                continue;
            }

            if( packageLocale != _locale )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PackageService::visitPackages( const LambdaPackage & _lambda ) const
    {
        for( const PackagePtr & package : m_packages )
        {
            _lambda( package );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::loadLocalePacksByName_( const ConstString & _locale, const Tags & _platformTags, VectorPackages * const _packs ) const
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

            if( _platformTags.hasTags( packagePlatform ) == false )
            {
                continue;
            }

            _packs->emplace_back( package );

            hasLocale = true;
        }

        return hasLocale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PackageService::enablePackages( const ConstString & _locale, const Tags & _platformTags )
    {
        LOGGER_INFO( "package", "Packages enable..." );

        VectorPackages packages;

        for( const PackagePtr & package : m_packages )
        {
            const Tags & packPlatform = package->getPlatfromTags();

            if( _platformTags.hasTags( packPlatform ) == false )
            {
                continue;
            }

            const ConstString & locale = package->getLocale();

            if( locale.empty() == false )
            {
                continue;
            }

            packages.emplace_back( package );
        }

        if( this->loadLocalePacksByName_( _locale, _platformTags, &packages ) == false )
        {
            if( this->loadLocalePacksByName_( STRINGIZE_STRING_LOCAL( "en" ), _platformTags, &packages ) == false )
            {
                LOGGER_WARNING( "not set locale pack" );
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

        if( NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_ENABLE_PACKAGES ) == false )
        {
            LOGGER_ERROR( "[resources validation is invalid]" );

            bool OPTION_resourceCheckCritical = HAS_OPTION( "noresourcecheckcritical" );

            if( OPTION_resourceCheckCritical == false )
            {
                LOGGER_CRITICAL( "Please fix resources and restart application!" );

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

            if( _platformTag.hasTags( platformTags ) == false )
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

            if( _platformTag.hasTags( platformTags ) == false )
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
        MENGINE_UNUSED( _prevLocale );
        MENGINE_UNUSED( _currentlocale );

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        if( this->disableLocalePackage( _prevLocale, platformTags ) == false )
        {
            LOGGER_ERROR( "invalid disable locale package '%s' platform '%s'"
                , _prevLocale.c_str()
                , Helper::tagsToString( platformTags ).c_str()
            );

            return;
        }

        if( this->enableLocalePackage( _currentlocale, platformTags ) == false )
        {
            LOGGER_ERROR( "invalid enable locale package '%s' platform '%s'"
                , _currentlocale.c_str()
                , Helper::tagsToString( platformTags ).c_str()
            );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}