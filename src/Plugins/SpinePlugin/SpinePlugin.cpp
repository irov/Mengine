#include "SpinePlugin.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"

#ifdef MENGINE_USE_PYTHON_FRAMEWORK
#include "SpineScriptEmbedding.h"
#endif

#include "ResourceSpineAtlasDefault.h"
#include "ResourceSpineAtlasTexturepacker.h"
#include "ResourceSpineSkeleton.h"

#include "Spine.h"

#include "LoaderResourceSpineAtlasDefault.h"
#include "LoaderResourceSpineAtlasTexturepacker.h"
#include "LoaderResourceSpineSkeleton.h"

#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Spine, Mengine::SpinePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
#ifdef MENGINE_USE_PYTHON_FRAMEWORK
        SERVICE_WAIT( ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "SpineScriptEmbedding" ), SpineScriptEmbedding );

            return true;
        } );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasDefault" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceSpineAtlasDefault, 64>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasTexturepacker" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceSpineAtlasTexturepacker, 64>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSpineSkeleton" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceSpineSkeleton, 64>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Spine" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Spine, 128>>() ) == false )
        {
            return false;
        }

        SERVICE_WAIT( LoaderServiceInterface, []()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasDefault" ), Helper::makeFactorableUnique<LoaderResourceSpineAtlasDefault>() );
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasTexturepacker" ), Helper::makeFactorableUnique<LoaderResourceSpineAtlasTexturepacker>() );
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceSpineSkeleton" ), Helper::makeFactorableUnique<LoaderResourceSpineSkeleton>() );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpinePlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_PYTHON_FRAMEWORK
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "SpineScriptEmbedding" ) );
        }
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