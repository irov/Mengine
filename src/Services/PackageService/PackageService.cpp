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
#include "Kernel/FileGroupHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PackageService, Mengine::PackageService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void configPackageDesc( const ConfigInterfacePtr & _config, const String & _name, PackageDesc * const _pack, bool _immediately )
        {
            _config->hasValue( _name.c_str(), "Name", ConstString::none(), &_pack->name );
            _config->hasValue( _name.c_str(), "Type", STRINGIZE_STRING_LOCAL( "dir" ), &_pack->type );
            _config->hasValue( _name.c_str(), "Locale", ConstString::none(), &_pack->locale );
            _config->hasValue( _name.c_str(), "Platform", Tags(), &_pack->platform );
            _config->hasValue( _name.c_str(), "Tag", Tags(), &_pack->tags );
            _config->hasValue( _name.c_str(), "Category", ConstString::none(), &_pack->fileGroupName ); //deprecated
            _config->hasValue( _name.c_str(), "FileGroup", ConstString::none(), &_pack->fileGroupName );
            _config->hasValue( _name.c_str(), "Parent", ConstString::none(), &_pack->parent );
            _config->hasValue( _name.c_str(), "Description", FilePath::none(), &_pack->descriptionPath );
            _config->hasValue( _name.c_str(), "Path", FilePath::none(), &_pack->path );
            _config->hasValue( _name.c_str(), "Fonts", FilePath::none(), &_pack->fontsPath );
            _config->hasValue( _name.c_str(), "Texts", FilePath::none(), &_pack->textsPath );
            _config->hasValue( _name.c_str(), "Dev", false, &_pack->dev );
            _config->hasValue( _name.c_str(), "PreLoad", true, &_pack->preload );
            _config->hasValue( _name.c_str(), "Immediately", _immediately, &_pack->immediately );
        }
        //////////////////////////////////////////////////////////////////////////
    }
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
        LOGGER_INFO( "package", "Packages load... %s"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _fileGroup, _filePath, ConstString::none(), _doc );

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

            if( config->hasSection( frameworkPack.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' framework package no found section for '%s'"
                    , _filePath.c_str()
                    , frameworkPack.c_str()
                );

                return false;
            }

            PackageDesc pack;
            Detail::configPackageDesc( config, frameworkPack, &pack, true );

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

            if( config->hasSection( resourcePack.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' resource package no found section for '%s'"
                    , _filePath.c_str()
                    , resourcePack.c_str()
                );

                return false;
            }

            PackageDesc pack;
            Detail::configPackageDesc( config, resourcePack, &pack, false );

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

            if( config->hasSection( languagePack.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' language package no found section for '%s'"
                    , _filePath.c_str()
                    , languagePack.c_str()
                );

                return false;
            }

            PackageDesc pack;
            Detail::configPackageDesc( config, languagePack, &pack, false );

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
            const PackageDesc & desc = package->getPackageDesc();

            if( desc.name != _name )
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

        if( package->initialize( baseFileGroup, _desc ) == false )
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

        const PackageDesc & desc = package->getPackageDesc();

        const Tags & packageTags = desc.platform;
        
        if( packageTags.empty() == true || platformTags.hasTags( packageTags ) )
        {
            if( package->load( _doc ) == false )
            {
                LOGGER_ERROR( "invalid load package '%s' path '%s'"
                    , package->getPackageDesc().name.c_str()
                    , package->getPackageDesc().path.c_str()
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

            const PackageDesc & desc = package->getPackageDesc();

            if( desc.name != _name )
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
            const PackageDesc & desc = package->getPackageDesc();

            if( desc.name != _name )
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
            const PackageDesc & desc = package->getPackageDesc();

            if( _platformTags.hasTags( desc.platform ) == false )
            {
                continue;
            }

            if( _locale.empty() == true )
            {
                return true;
            }

            if( desc.locale.empty() == true )
            {
                continue;
            }

            if( desc.locale != _locale )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PackageService::foreachPackages( const LambdaPackage & _lambda ) const
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
            const PackageDesc & desc = package->getPackageDesc();

            if( desc.locale != _locale )
            {
                continue;
            }

            if( _platformTags.hasTags( desc.platform ) == false )
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
            const PackageDesc & desc = package->getPackageDesc();

            if( _platformTags.hasTags( desc.platform ) == false )
            {
                continue;
            }

            if( desc.locale.empty() == false )
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
            const PackageDesc & desc = package->getPackageDesc();

            if( desc.preload == false )
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
            const PackageDesc & desc = package->getPackageDesc();

            if( _platformTag.hasTags( desc.platform ) == false )
            {
                continue;
            }

            if( desc.locale != _locale )
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
            const PackageDesc & desc = package->getPackageDesc();

            if( _platformTag.hasTags( desc.platform ) == false )
            {
                continue;
            }

            if( desc.locale != _locale )
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