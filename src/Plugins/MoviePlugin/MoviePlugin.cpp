#include "MoviePlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Plugins/NodeDebugRenderPlugin/NodeDebugRenderServiceInterface.h"
#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"
#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "Movie2ScriptEmbedding.h"
#endif

#include "DataflowAEZ.h"
#include "Movie2DebugRender.h"

#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

#include "Movie2.h"
#include "ResourceMovie2.h"
#include "LoaderResourceMovie2.h"
#include "Movie2Slot.h"

#include "ResourceMovie2Validator.h"
#include "Movie2DebuggerBoundingBox.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie, Mengine::MoviePlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t stdex_movie_alloc( ae_userdata_t _userdata, ae_size_t _size )
    {
        AE_UNUSED( _userdata );

        return ALLOCATOR_SERVICE()
            ->malloc( _size, "movie" );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t stdex_movie_alloc_n( ae_userdata_t _userdata, ae_size_t _size, ae_size_t _count )
    {
        AE_UNUSED( _userdata );

        size_t total = _size * _count;

        return ALLOCATOR_SERVICE()
            ->malloc( total, "movie" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_free( ae_userdata_t _userdata, ae_constvoidptr_t _ptr )
    {
        AE_UNUSED( _userdata );

        ALLOCATOR_SERVICE()
            ->free( (void *)_ptr, "movie" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_free_n( ae_userdata_t _userdata, ae_constvoidptr_t _ptr )
    {
        AE_UNUSED( _userdata );

        ALLOCATOR_SERVICE()
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
            LOGGER_ERROR( "invalid format error '%s' code '%d'"
                , _format
                , _code
            );

            return;
        }

        LOGGER_ERROR( "error '%s' code '%d'"
            , msg
            , _code
        );
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
        const Char * hashkey = CONFIG_VALUE( "MoviePlugin", "HASHKEY", "" );

        m_hashkey = hashkey;

        m_movieInstance = ae_create_movie_instance( m_hashkey.c_str(), &stdex_movie_alloc, &stdex_movie_alloc_n, &stdex_movie_free, &stdex_movie_free_n, 0, &stdex_movie_logerror, this );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [MENGINE_DEBUG_ARGUMENTS( this )]()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "Movie2ScriptEmbedding" ), Helper::makeFactorableUnique<Movie2ScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "Movie2ScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Movie2, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Movie2Slot, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

#ifdef MENGINE_USE_SCRIPT_SERVICE
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<Movie2SubComposition, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }
#else
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), Helper::makeFactorableUnique<DefaultPrototypeGenerator<Movie2SubComposition, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }
#endif

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

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceMovie2, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcherDataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), resourcePrefetcherDataflow, MENGINE_DOCUMENT_FACTORABLE );
        }

        if( SERVICE_EXIST( NodeDebugRenderServiceInterface ) == true )
        {
            NODEDEBUGRENDER_SERVICE()
                ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<Movie2DebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );
        }

        PLUGIN_SERVICE_WAIT( ResourceValidateServiceInterface, [this]()
        {
            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( archivator );

            ResourceMovie2ValidatorPtr movie2Validator = Helper::makeFactorableUnique<ResourceMovie2Validator>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( movie2Validator );

            movie2Validator->setMovieInstance( m_movieInstance );
            movie2Validator->setArchivator( archivator );

            VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), movie2Validator, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_WAIT( LoaderServiceInterface, [MENGINE_DEBUG_ARGUMENTS( this )]()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<LoaderResourceMovie2>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( LoaderServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        } );

        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<Movie2DebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MoviePlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        ae_delete_movie_instance( m_movieInstance );
        m_movieInstance = nullptr;

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        }

        if( SERVICE_EXIST( NodeDebugRenderServiceInterface ) == true )
        {
            NODEDEBUGRENDER_SERVICE()
                ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "Movie2" ) );
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        }

#ifdef STDEX_ALLOCATOR_REPORT_ENABLE
        uint32_t report_count = stdex_get_allocator_report_count( "movie" );
        MENGINE_ASSERTION( report_count == 0, "Movie memleak [%d]"
            , report_count
        );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
