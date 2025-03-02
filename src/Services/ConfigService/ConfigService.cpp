#include "ConfigService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/FileStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdAlgorithm.h"

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
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        m_mutex = mutex;

        m_factoryMemoryConfig = Helper::makeFactoryPool<MemoryConfig, 16>( MENGINE_DOCUMENT_FACTORABLE );

        MultiConfigPtr defaultConfig = Helper::makeFactorableUnique<MultiConfig>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( defaultConfig, "invalid create default config" );

        defaultConfig->setMutex( mutex );

        m_defaultConfig = defaultConfig;

#if !defined(MENGINE_BUILD_PUBLISH_ENABLE)
        PersistentConfigPtr persistentConfig = Helper::makeFactorableUnique<PersistentConfig>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( persistentConfig, "invalid create persistent config" );

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

#if !defined(MENGINE_BUILD_PUBLISH_ENABLE)
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

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid create memory config" );

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        config->setPlatformTags( platformTags );

        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr ConfigService::loadConfig( const ContentInterfacePtr & _content, const ConstString & _configType, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "config", "load config '%s' (doc: %s)"
            , Helper::getContentFullPath( _content ).c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, _doc );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "invalid open config '%s' (doc: %s)"
                , Helper::getContentFullPath( _content ).c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        if( stream->size() == 0 )
        {
            LOGGER_ERROR( "empty config '%s' (doc: %s)"
                , Helper::getContentFullPath( _content ).c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        ConstString configType = _configType;

        if( _configType == ConstString::none() )
        {
            const FilePath & filePath = _content->getFilePath();

            configType = Helper::getFilePathExt( filePath );
        }

        ConfigInterfacePtr config = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Config" ), configType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid generate config '%s' (doc: %s)"
            , Helper::getContentFullPath( _content ).c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        config->setPlatformTags( platformTags );

        if( config->load( stream, _doc ) == false )
        {
            LOGGER_ERROR( "invalid load config '%s' (doc: %s)"
                , Helper::getContentFullPath( _content ).c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConfigService::loadDefaultConfig( const ContentInterfacePtr & _content, const ConstString & _configType, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "config", "load default config '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        ConfigInterfacePtr config = this->loadConfig( _content, _configType, _doc );

        if( config == nullptr )
        {
            LOGGER_ERROR( "invalid load default config '%s'"
                , Helper::getContentFullPath( _content ).c_str()
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
#if !defined(MENGINE_BUILD_PUBLISH_ENABLE)
    //////////////////////////////////////////////////////////////////////////
    const ConfigInterfacePtr & ConfigService::getPersistentConfig() const
    {
        return m_persistentConfig;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}
