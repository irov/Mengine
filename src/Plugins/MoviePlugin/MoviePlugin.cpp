#include "MoviePlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Plugins/NodeDebugRenderPlugin/NodeDebugRenderServiceInterface.h"
#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"
#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "Movie2ScriptEmbedding.h"
#endif

#include "DataflowAEZ.h"
#include "Movie2DebugRender.h"

#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotPolygon.h"

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

#include "stdex/allocator_report.h"

#include <stdlib.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie, Mengine::MoviePlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t stdex_movie_alloc( ae_userdata_t _userdata, ae_size_t _size )
    {
        AE_UNUSED( _userdata );

        return stdex_malloc( _size, "movie" );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t stdex_movie_alloc_n( ae_userdata_t _userdata, ae_size_t _size, ae_size_t _count )
    {
        AE_UNUSED( _userdata );

        size_t total = _size * _count;

        return stdex_malloc( total, "movie" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_free( ae_userdata_t _userdata, ae_constvoidptr_t _ptr )
    {
        AE_UNUSED( _userdata );

        stdex_free( (void *)_ptr, "movie" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_free_n( ae_userdata_t _userdata, ae_constvoidptr_t _ptr )
    {
        AE_UNUSED( _userdata );

        stdex_free( (void *)_ptr, "movie" );
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

        va_list argList;

        va_start( argList, _format );
        Char msg[4096];
        vsprintf( msg, _format, argList );
        va_end( argList );

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
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, []()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "Movie2ScriptEmbedding" ), Helper::makeFactorableUnique<Movie2ScriptEmbedding>() );
        } );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "Movie2ScriptEmbedding" ) );
        } );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Movie2, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Movie2Slot, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<Movie2SubComposition, 128>>() ) == false )
        {
            return false;
        }

        SERVICE_WAIT( DataServiceInterface, [this]()
        {
            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

            DataflowAEZPtr dataflowAEZ = Helper::makeFactorableUnique<DataflowAEZ>();

            MENGINE_ASSERTION_MEMORY_PANIC( dataflowAEZ, false );

            dataflowAEZ->setMovieInstance( m_movieInstance );
            dataflowAEZ->setArchivator( archivator );

            if( dataflowAEZ->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aezMovie" ), dataflowAEZ );

            return true;
        } );

        SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflow = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aezMovie" ) );
            dataflow->finalize();
        } );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceMovie2, 128>>() ) == false )
        {
            return false;
        }

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcher = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );

            RESOURCEPREFETCHER_SERVICE()
                ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), resourcePrefetcher );
        }

        if( SERVICE_EXIST( NodeDebugRenderServiceInterface ) == true )
        {
            NODEDEBUGRENDER_SERVICE()
                ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<Movie2DebugRender>() );
        }

        SERVICE_WAIT( ResourceValidateServiceInterface, [this]()
        {
            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

            ResourceMovie2ValidatorPtr movie2Validator = Helper::makeFactorableUnique<ResourceMovie2Validator>();

            movie2Validator->setMovieInstance( m_movieInstance );
            movie2Validator->setArchivator( archivator );

            VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), movie2Validator );

            return true;
        } );

        SERVICE_WAIT( LoaderServiceInterface, []()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<LoaderResourceMovie2>() );

            return true;
        } );

        VOCABULARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<Movie2DebuggerBoundingBox>() );

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

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            RESOURCEPREFETCHER_SERVICE()
                ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
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

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        }

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );

#ifdef STDEX_ALLOCATOR_REPORT_ENABLE
        uint32_t report_count = stdex_get_allocator_report_count( "movie" );
        MENGINE_ASSERTION( report_count == 0, "Movie memleak [%d]"
            , report_count
        );
#endif
    }
}
