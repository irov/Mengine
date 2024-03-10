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

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#include "Movie2ScriptEmbedding.h"
#endif

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

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie, Mengine::MoviePlugin )
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

            Char msg[4096] = {'\0'};

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
        const Char * MoviePlugin_HASHKEY = CONFIG_VALUE( "MoviePlugin", "HASHKEY", "" );

        m_hashkey = MoviePlugin_HASHKEY;

        m_movieInstance = ae_create_movie_instance( m_hashkey.c_str(), &Detail::stdex_movie_alloc, &Detail::stdex_movie_alloc_n, &Detail::stdex_movie_free, &Detail::stdex_movie_free_n, 0, &Detail::stdex_movie_logerror, this );

#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [MENGINE_DEBUG_ARGUMENTS( this )]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "Movie2ScriptEmbedding" ), Helper::makeFactorableUnique<Movie2ScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "Movie2ScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( Helper::addNodePrototype<Movie2, 128>( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<Movie2Slot, 128>( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addIdentityPrototype<Movie2SubComposition, 128>( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [this]()
        {
            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( archivator );

            DataflowAEZPtr dataflowAEZ = Helper::makeFactorableUnique<DataflowAEZ>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( dataflowAEZ );

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

        if( Helper::addResourcePrototype<ResourceMovie2, 128>( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( ResourcePrefetcherServiceInterface, [MENGINE_DOCUMENT_ARGUMENTS( this )]()
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcherDataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), resourcePrefetcherDataflow, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourcePrefetcherServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        } );

        PLUGIN_SERVICE_WAIT( NodeDebugRenderServiceInterface, [MENGINE_DOCUMENT_ARGUMENTS( this )]()
        {
            NODEDEBUGRENDER_SERVICE()
                ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<Movie2DebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( NodeDebugRenderServiceInterface, []()
        {
            NODEDEBUGRENDER_SERVICE()
                ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "Movie2" ) );
        } );

        PLUGIN_SERVICE_WAIT( ResourceValidateServiceInterface, [this]()
        {
            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( archivator );

            ResourceMovie2ValidatorPtr movie2Validator = Helper::makeFactorableUnique<ResourceMovie2Validator>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( movie2Validator );

            movie2Validator->setMovieInstance( m_movieInstance );
            movie2Validator->setArchivator( archivator );

            VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), movie2Validator, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourceValidateServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        } );

        PLUGIN_SERVICE_WAIT( LoaderServiceInterface, [MENGINE_DEBUG_ARGUMENTS( this )]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<LoaderResourceMovie2>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( LoaderServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        } );

        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<Movie2DebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MoviePlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        ae_delete_movie_instance( m_movieInstance );
        m_movieInstance = nullptr;

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );

        MENGINE_ASSERTION_ALLOCATOR( "movie" );
    }
    //////////////////////////////////////////////////////////////////////////
}
