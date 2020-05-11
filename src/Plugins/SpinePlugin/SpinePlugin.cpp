#include "SpinePlugin.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "SpineScriptEmbedding.h"
#endif

#include "ResourceSpineAtlasDefault.h"
#include "ResourceSpineAtlasTexturepacker.h"
#include "ResourceSpineSkeleton.h"

#include "Spine.h"

#include "LoaderResourceSpineAtlasDefault.h"
#include "LoaderResourceSpineAtlasTexturepacker.h"
#include "LoaderResourceSpineSkeleton.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"

#include "spine/extension.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Spine, Mengine::SpinePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * my_spine_malloc( size_t size )
        {
            void * p = Helper::allocateMemory( size, "spine" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * my_spine_debug_malloc( size_t size, const char * file, int line )
        {
            MENGINE_UNUSED( file );
            MENGINE_UNUSED( line );

            void * p = Helper::allocateMemory( size, "spine" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * my_spine_realloc( void * ptr, size_t size )
        {
            void * p = Helper::reallocateMemory( ptr, size, "spine" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void my_spine_free( void * ptr )
        {
            Helper::deallocateMemory( ptr, "spine" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SpinePlugin::SpinePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SpinePlugin::~SpinePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpinePlugin::_initializePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [this]()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "SpineScriptEmbedding" ), Helper::makeFactorableUnique<SpineScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        } );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "SpineScriptEmbedding" ) );
        } );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasDefault" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceSpineAtlasDefault, 64>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasTexturepacker" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceSpineAtlasTexturepacker, 64>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineSkeleton" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceSpineSkeleton, 64>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Spine" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Spine, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        SERVICE_WAIT( LoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasDefault" ), Helper::makeFactorableUnique<LoaderResourceSpineAtlasDefault>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasTexturepacker" ), Helper::makeFactorableUnique<LoaderResourceSpineAtlasTexturepacker>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineSkeleton" ), Helper::makeFactorableUnique<LoaderResourceSpineSkeleton>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        _spSetMalloc( &Detail::my_spine_malloc );
        _spSetDebugMalloc( &Detail::my_spine_debug_malloc );
        _spSetRealloc( &Detail::my_spine_realloc );
        _spSetFree( &Detail::my_spine_free );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpinePlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING_END );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasDefault" ) );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasTexturepacker" ) );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineSkeleton" ) );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasDefault" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasTexturepacker" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineSkeleton" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Spine" ) );
    }
}