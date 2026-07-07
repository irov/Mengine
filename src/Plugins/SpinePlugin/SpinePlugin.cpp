#include "SpinePlugin.h"

#include "Configuration/Configurations.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#if defined(MENGINE_PLUGIN_METABUF)
#include "Interface/MetabufLoaderServiceInterface.h"
#endif

#include "Interface/DebuggerBoundingBoxInterface.h"

#include "ResourceSpineAtlasDefault.h"
#include "ResourceSpineAtlasTexturepacker.h"
#include "ResourceSpineSkeleton.h"

#include "SpineDebuggerBoundingBox.h"

#include "Spine.h"

#if defined(MENGINE_PLUGIN_METABUF)
#include "MetabufLoaderResourceSpineAtlasDefault.h"
#include "MetabufLoaderResourceSpineAtlasTexturepacker.h"
#include "MetabufLoaderResourceSpineSkeleton.h"
#endif

#if defined(MENGINE_PLUGIN_JSON)
#include "JSONLoaderResourceSpineAtlasDefault.h"
#include "JSONLoaderResourceSpineAtlasTexturepacker.h"
#include "JSONLoaderResourceSpineSkeleton.h"
#endif

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

#if defined(MENGINE_PLUGIN_METABUF)
        PLUGIN_SERVICE_WAIT( MetabufLoaderServiceInterface, [this]()
        {
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineAtlasDefault::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceSpineAtlasDefault>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineAtlasTexturepacker::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceSpineAtlasTexturepacker>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
            VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineSkeleton::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceSpineSkeleton>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( MetabufLoaderServiceInterface, [this]()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineAtlasDefault::getFactorableType() );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineAtlasTexturepacker::getFactorableType() );
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceSpineSkeleton::getFactorableType() );
        } );
#endif

#if defined(MENGINE_PLUGIN_JSON)
        VOCABULARY_SET( JSONLoaderInterface, STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceSpineAtlasDefault::getFactorableType(), Helper::makeFactorableUnique<JSONLoaderResourceSpineAtlasDefault>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( JSONLoaderInterface, STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceSpineAtlasTexturepacker::getFactorableType(), Helper::makeFactorableUnique<JSONLoaderResourceSpineAtlasTexturepacker>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( JSONLoaderInterface, STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceSpineSkeleton::getFactorableType(), Helper::makeFactorableUnique<JSONLoaderResourceSpineSkeleton>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
#endif

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
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), Spine::getFactorableType() );

#if defined(MENGINE_PLUGIN_JSON)
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceSpineAtlasDefault::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceSpineAtlasTexturepacker::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceSpineSkeleton::getFactorableType() );
#endif

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
