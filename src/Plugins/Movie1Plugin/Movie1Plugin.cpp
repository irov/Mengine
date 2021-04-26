#include "Movie1Plugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "MovieScriptEmbedding.h"
#endif

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"
#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "Engine/HotSpot.h"
#include "Engine/HotSpotResourceShape.h"
#include "Engine/ResourceShape.h"

#include "Kernel/RenderScissor.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/PolygonHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/Logger.h"

#include "Movie.h"
#include "MovieSlot.h"
#include "MovieSceneEffect.h"
#include "MovieMesh2D.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "MovieInternalObject.h"
#   include "MovieEvent.h"
#endif

#include "ResourceMovie.h"
#include "ResourceInternalObject.h"

#include "LoaderResourceMovie.h"

#include "DataflowAEK.h"

#include "ResourceMovieValidator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie1, Mengine::Movie1Plugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie1Plugin::Movie1Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie1Plugin::~Movie1Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie1Plugin::_availablePlugin() const
    {
        bool available = CONFIG_VALUE( "Engine", "Movie1PluginAvailable", true );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie1Plugin::_initializePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [MENGINE_DEBUG_ARGUMENTS( this )]()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "MovieScriptEmbedding" ), Helper::makeFactorableUnique<MovieScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "MovieScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Movie, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSlot" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieSlot, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieSceneEffect, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

#ifdef MENGINE_USE_SCRIPT_SERVICE
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieInternalObject" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieInternalObject, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieEvent" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieEvent, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieMesh2D" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieMesh2D, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceMovie, 64>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceInternalObject" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceInternalObject, 64>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [MENGINE_DEBUG_ARGUMENTS( this )]()
        {
            DataflowInterfacePtr dataflowAEK = Helper::makeFactorableUnique<DataflowAEK>( MENGINE_DOCUMENT_FACTORABLE );

            if( dataflowAEK->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aekMovie" ), dataflowAEK, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflow = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aekMovie" ) );
            dataflow->finalize();
        } );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "aek" ), STRINGIZE_STRING_LOCAL( "aekMovie" ) );

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcherDataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ), resourcePrefetcherDataflow, MENGINE_DOCUMENT_FACTORABLE );
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ), Helper::makeFactorableUnique<ResourceMovieValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        }

        PLUGIN_SERVICE_WAIT( LoaderServiceInterface, [MENGINE_DEBUG_ARGUMENTS( this )]()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ), Helper::makeFactorableUnique<LoaderResourceMovie>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie1Plugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSlot" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ) );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieInternalObject" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieEvent" ) );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieMesh2D" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceInternalObject" ) );

        CODEC_UNREGISTER_EXT( STRINGIZE_STRING_LOCAL( "aek" ) );

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );
        }

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie1Plugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "movie1" );
    }
    //////////////////////////////////////////////////////////////////////////
}
