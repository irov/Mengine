#include "ConfigService.h"

#include "Interface/PlatformInterface.h"

#include "Kernel/Exception.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FileGroupHelper.h"

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
    bool ConfigService::_initializeService()
    {
        m_factoryMemoryConfig = Helper::makeFactoryPool<MemoryConfig, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryIniConfig = Helper::makeFactoryPool<IniConfig, 16>( MENGINE_DOCUMENT_FACTORABLE );

        m_defaultConfig = m_factoryIniConfig->createObject( MENGINE_DOCUMENT_FACTORABLE );

        SERVICE_WAIT( PlatformInterface, [this]()
        {
            const Tags & platformTags = PLATFORM_SERVICE()
                ->getPlatformTags();

            m_platformTags = platformTags;

            m_defaultConfig->setPlatformTags( m_platformTags );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::_finalizeService()
    {
        m_platformTags.clear();

        m_defaultConfig = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryConfig );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryIniConfig );

        m_factoryMemoryConfig = nullptr;
        m_factoryIniConfig = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr ConfigService::createMemoryConfig( const DocumentPtr & _doc )
    {
        MemoryConfigPtr config = m_factoryMemoryConfig->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config );

        config->setPlatformTags( m_platformTags );

        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr ConfigService::loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "config", "load config '%s' (doc: %s)"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            , MENGINE_DOCUMENT_STR( _doc )
        );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open config '%s'"
            , _filePath.c_str()
        );

        INIConfigPtr config = m_factoryIniConfig->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config );

        config->setPlatformTags( m_platformTags );

        if( config->load( stream, _doc ) == false )
        {
            LOGGER_ERROR( "invalid load config '%s'"
                , _filePath.c_str()
            );

            return nullptr;
        }

        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConfigService::loadDefaultConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "config", "load default config '%s'"
            , _filePath.c_str()
        );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open config '%s'"
            , _filePath.c_str()
        );

        if( m_defaultConfig->load( stream, _doc ) == false )
        {
            LOGGER_ERROR( "invalid load config '%s'"
                , _filePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConfigInterfacePtr & ConfigService::getDefaultConfig() const
    {
        return m_defaultConfig;
    }
    //////////////////////////////////////////////////////////////////////////
}
