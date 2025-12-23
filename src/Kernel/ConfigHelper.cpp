#include "ConfigHelper.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ConfigInterfacePtr loadConfig( const ContentInterfacePtr & _content, const ConstString & _configType, const DocumentInterfacePtr & _doc )
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

            ConfigInterfacePtr config = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Config" ), configType, _doc );

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

            _content->closeInputStreamFile( stream );

            return config;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}