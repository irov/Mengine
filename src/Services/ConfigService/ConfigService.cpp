#include "ConfigService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Exception.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/FilePathHelper.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ConfigService, Mengine::ConfigService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConfigService::ConfigService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigService::~ConfigService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & ConfigService::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( ThreadSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConfigService::_initializeService()
    {
        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        m_factoryMemoryConfig = Helper::makeFactoryPool<MemoryConfig, 16>( MENGINE_DOCUMENT_FACTORABLE );

        MultiConfigPtr defaultConfig = Helper::makeFactorableUnique<MultiConfig>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( defaultConfig );

        defaultConfig->setMutex( mutex );

        m_defaultConfig = defaultConfig;

#ifndef MENGINE_BUILD_PUBLISH
        PersistentConfigPtr persistentConfig = Helper::makeFactorableUnique<PersistentConfig>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( persistentConfig );

        persistentConfig->setMutex( mutex );

        m_defaultConfig->addConfig( persistentConfig );

        m_persistentConfig = persistentConfig;
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::_finalizeService()
    {
        if( m_defaultConfig != nullptr )
        {
            m_defaultConfig->setMutex( nullptr );
            m_defaultConfig = nullptr;
        }

#ifndef MENGINE_BUILD_PUBLISH
        if( m_persistentConfig != nullptr )
        {
            m_persistentConfig->setMutex( nullptr );
            m_persistentConfig = nullptr;
        }
#endif

        m_mutex = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryConfig );

        m_factoryMemoryConfig = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr ConfigService::createMemoryConfig( const DocumentInterfacePtr & _doc )
    {
        MemoryConfigPtr config = m_factoryMemoryConfig->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config );

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        config->setPlatformTags( platformTags );

        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr ConfigService::loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "config", "load config '%s' (doc: %s)"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            , MENGINE_DOCUMENT_STR( _doc )
        );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open config '%s' (doc: %s)"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            , MENGINE_DOCUMENT_STR( _doc )
        );

        ConstString configType = _configType;

        if( _configType == ConstString::none() )
        {
            configType = Helper::getFilePathExt( _filePath );
        }

        ConfigInterfacePtr config = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Config" ), configType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config );

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        config->setPlatformTags( platformTags );

        if( config->load( stream, _doc ) == false )
        {
            LOGGER_ERROR( "invalid load config '%s' (doc: %s)"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConfigService::loadDefaultConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "config", "load default config '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        ConfigInterfacePtr config = this->loadConfig( _fileGroup, _filePath, _configType, _doc );

        if( config == nullptr )
        {
            LOGGER_ERROR( "invalid load default config '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            return false;
        }

        m_defaultConfig->addConfig( config );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConfigInterfacePtr & ConfigService::getDefaultConfig() const
    {
        return m_defaultConfig;
    }
    //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_BUILD_PUBLISH
    //////////////////////////////////////////////////////////////////////////
    const ConfigInterfacePtr & ConfigService::getPersistentConfig() const
    {
        return m_persistentConfig;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}
