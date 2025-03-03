#include "SpinePlugin.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/DebuggerBoundingBoxInterface.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#include "SpineScriptEmbedding.h"
#endif

#include "ResourceSpineAtlasDefault.h"
#include "ResourceSpineAtlasTexturepacker.h"
#include "ResourceSpineSkeleton.h"

#include "SpineDebuggerBoundingBox.h"

#include "Spine.h"

#include "LoaderResourceSpineAtlasDefault.h"
#include "LoaderResourceSpineAtlasTexturepacker.h"
#include "LoaderResourceSpineSkeleton.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/IdentityPrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

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
        //////////////////////////////////////////////////////////////////////////
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
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( SpineScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<SpineScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( SpineScriptEmbedding::getFactorableType() );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( Helper::addResourcePrototype<ResourceSpineAtlasDefault, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceSpineAtlasTexturepacker, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceSpineSkeleton, 64>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addIdentityPrototype<SamplerSpineAnimation, 128>( Node::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<Spine, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( LoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineAtlasDefault::getFactorableType(), Helper::makeFactorableUnique<LoaderResourceSpineAtlasDefault>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineAtlasTexturepacker::getFactorableType(), Helper::makeFactorableUnique<LoaderResourceSpineAtlasTexturepacker>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineSkeleton::getFactorableType(), Helper::makeFactorableUnique<LoaderResourceSpineSkeleton>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( LoaderServiceInterface, [this]()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineAtlasDefault::getFactorableType() );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineAtlasTexturepacker::getFactorableType() );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineSkeleton::getFactorableType() );
        } );

        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), Spine::getFactorableType(), Helper::makeFactorableUnique<SpineDebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        _spSetMalloc( &Detail::my_spine_malloc );
        _spSetDebugMalloc( &Detail::my_spine_debug_malloc );
        _spSetRealloc( &Detail::my_spine_realloc );
        _spSetFree( &Detail::my_spine_free );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpinePlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), Spine::getFactorableType() );

        Helper::removeResourcePrototype<ResourceSpineAtlasDefault>();
        Helper::removeResourcePrototype<ResourceSpineAtlasTexturepacker>();
        Helper::removeResourcePrototype<ResourceSpineSkeleton>();

        Helper::removeNodePrototype<SamplerSpineAnimation>();
        Helper::removeNodePrototype<Spine>();

        spAnimationState_disposeStatics();
    }
    //////////////////////////////////////////////////////////////////////////
    void SpinePlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "spine" );
    }
    //////////////////////////////////////////////////////////////////////////
}