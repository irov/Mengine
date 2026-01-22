#include "MoviePlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/AllocatorSystemInterface.h"

#include "Plugins/NodeDebugRenderPlugin/NodeDebugRenderServiceInterface.h"
#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"
#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "DataflowAEZ.h"
#include "Movie2DebugRender.h"

#include "Movie2.h"
#include "ResourceMovie2.h"
#include "LoaderResourceMovie2.h"
#include "Movie2Slot.h"

#include "ResourceMovie2Validator.h"
#include "Movie2DebuggerBoundingBox.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie, Mengine::MoviePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static ae_voidptr_t stdex_movie_alloc( ae_userdata_t _userdata, ae_size_t _size )
        {
            AE_UNUSED( _userdata );

            return ALLOCATOR_SYSTEM()
                ->malloc( _size, "movie" );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_voidptr_t stdex_movie_alloc_n( ae_userdata_t _userdata, ae_size_t _size, ae_size_t _count )
        {
            AE_UNUSED( _userdata );

            size_t total = _size * _count;

            return ALLOCATOR_SYSTEM()
                ->malloc( total, "movie" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void stdex_movie_free( ae_userdata_t _userdata, ae_constvoidptr_t _ptr )
        {
            AE_UNUSED( _userdata );

            ALLOCATOR_SYSTEM()
                ->free( (void *)_ptr, "movie" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void stdex_movie_free_n( ae_userdata_t _userdata, ae_constvoidptr_t _ptr )
        {
            AE_UNUSED( _userdata );

            ALLOCATOR_SYSTEM()
                ->free( (void *)_ptr, "movie" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void stdex_movie_logerror( ae_userdata_t _userdata, aeMovieErrorCode _code, const ae_char_t * _format, ... )
        {
            AE_UNUSED( _userdata );

            switch( _code )
            {
            case AE_ERROR_STREAM:
                {
                    return;
                }break;
            default:
                {
                }break;
            }

            Char msg[4096 + 1] = {'\0'};

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );
            int32_t size_vsnprintf = MENGINE_VSNPRINTF( msg, 4096, _format, args );
            MENGINE_VA_LIST_END( args );

            if( size_vsnprintf < 0 )
            {
                LOGGER_ERROR( "invalid format error '%s' code [%u]"
                    , _format
                    , _code
                );

                return;
            }

            LOGGER_ERROR( "error '%s' code [%u]"
                , msg
                , _code
            );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    MoviePlugin::MoviePlugin()
        : m_movieInstance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MoviePlugin::~MoviePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MoviePlugin::_initializePlugin()
    {
        String MoviePlugin_HASHKEY = CONFIG_VALUE_STRING( "MoviePlugin", "HASHKEY", "" );

        m_hashkey = MoviePlugin_HASHKEY;

        m_movieInstance = ae_create_movie_instance( m_hashkey.c_str(), &Detail::stdex_movie_alloc, &Detail::stdex_movie_alloc_n, &Detail::stdex_movie_free, &Detail::stdex_movie_free_n, 0, &Detail::stdex_movie_logerror, this );

        if( Helper::addNodePrototype<Movie2, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<Movie2Slot, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addIdentityPrototype<Movie2SubComposition, 128>( Node::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [this]()
        {
            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( archivator, "not found 'lz4' archivator" );

            DataflowAEZPtr dataflowAEZ = Helper::makeFactorableUnique<DataflowAEZ>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( dataflowAEZ, "invalid create 'DataflowAEZ'" );

            dataflowAEZ->setMovieInstance( m_movieInstance );
            dataflowAEZ->setArchivator( archivator );

            if( dataflowAEZ->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aezMovie" ), dataflowAEZ, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflow = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aezMovie" ) );
            dataflow->finalize();
        } );

        if( Helper::addResourcePrototype<ResourceMovie2, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( ResourcePrefetcherServiceInterface, [MENGINE_DOCUMENT_ARGUMENTS( this )]()
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcherDataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceMovie2::getFactorableType(), resourcePrefetcherDataflow, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourcePrefetcherServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceMovie2::getFactorableType() );
        } );

        PLUGIN_SERVICE_WAIT( NodeDebugRenderServiceInterface, [MENGINE_DOCUMENT_ARGUMENTS( this )]()
        {
            NODEDEBUGRENDER_SERVICE()
                ->addNodeDebugRender( Movie2::getFactorableType(), Helper::makeFactorableUnique<Movie2DebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( NodeDebugRenderServiceInterface, []()
        {
            NODEDEBUGRENDER_SERVICE()
                ->removeNodeDebugRender( Movie2::getFactorableType() );
        } );

        PLUGIN_SERVICE_WAIT( ResourceValidateServiceInterface, [this]()
        {
            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( archivator, "not found 'lz4' archivator" );

            ResourceMovie2ValidatorPtr movie2Validator = Helper::makeFactorableUnique<ResourceMovie2Validator>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( movie2Validator, "invalid create 'ResourceMovie2Validator'" );

            movie2Validator->setMovieInstance( m_movieInstance );
            movie2Validator->setArchivator( archivator );

            VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceMovie2::getFactorableType(), movie2Validator, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourceValidateServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceMovie2::getFactorableType() );
        } );

        PLUGIN_SERVICE_WAIT( LoaderServiceInterface, [MENGINE_DOCUMENT_ARGUMENTS( this )]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceMovie2::getFactorableType(), Helper::makeFactorableUnique<LoaderResourceMovie2>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( LoaderServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceMovie2::getFactorableType() );
        } );

        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), Movie2::getFactorableType(), Helper::makeFactorableUnique<Movie2DebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MoviePlugin::_finalizePlugin()
    {
        ae_delete_movie_instance( m_movieInstance );
        m_movieInstance = nullptr;

        Helper::removeNodePrototype<Movie2>();
        Helper::removeNodePrototype<Movie2Slot>();
        Helper::removeNodePrototype<Movie2SubComposition>();
        Helper::removeResourcePrototype<ResourceMovie2>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), Movie2::getFactorableType() );

        MENGINE_ASSERTION_ALLOCATOR( "movie" );
    }
    //////////////////////////////////////////////////////////////////////////
}
