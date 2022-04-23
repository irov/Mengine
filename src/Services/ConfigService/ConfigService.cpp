#include "ConfigService.h"

#include "Interface/PlatformInterface.h"
#include "Interface/PrototypeServiceInterface.h"

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
    bool ConfigService::_initializeService()
    {
        m_factoryMemoryConfig = Helper::makeFactoryPool<MemoryConfig, 16>( MENGINE_DOCUMENT_FACTORABLE );

        m_defaultConfig = Helper::makeFactorableUnique<MultiConfig>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::_finalizeService()
    {
        m_defaultConfig = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryConfig );

        m_factoryMemoryConfig = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr ConfigService::createMemoryConfig( const DocumentPtr & _doc )
    {
        MemoryConfigPtr config = m_factoryMemoryConfig->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config );

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        config->setPlatformTags( platformTags );

        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr ConfigService::loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentPtr & _doc )
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
    bool ConfigService::loadDefaultConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "config", "load default config '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        ConfigInterfacePtr config = this->loadConfig( _fileGroup, _filePath, _configType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load default config '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        m_defaultConfig->addConfig( config );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConfigInterfacePtr & ConfigService::getDefaultConfig() const
    {
        return m_defaultConfig;
    }
    //////////////////////////////////////////////////////////////////////////
}
