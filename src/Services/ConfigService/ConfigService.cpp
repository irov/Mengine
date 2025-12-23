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

        MultiConfigPtr mainConfig = Helper::makeFactorableUnique<MultiConfig>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mainConfig, "invalid create default config" );

        mainConfig->setMutex( mutex );

        m_mainConfig = mainConfig;

#if !defined(MENGINE_BUILD_PUBLISH_ENABLE)
        PersistentConfigPtr persistentConfig = Helper::makeFactorableUnique<PersistentConfig>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( persistentConfig, "invalid create persistent config" );

        persistentConfig->setMutex( mutex );

        m_mainConfig->addBackConfig( persistentConfig );

        m_persistentConfig = persistentConfig;
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::_finalizeService()
    {
        if( m_mainConfig != nullptr )
        {
            m_mainConfig->setMutex( nullptr );
            m_mainConfig = nullptr;
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
    void ConfigService::addFrontConfig( const ConfigInterfacePtr & _config )
    {
        m_mainConfig->addFrontConfig( _config );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::addBackConfig( const ConfigInterfacePtr & _config )
    {
        m_mainConfig->addBackConfig( _config );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConfigInterfacePtr & ConfigService::getMainConfig() const
    {
        return m_mainConfig;
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
