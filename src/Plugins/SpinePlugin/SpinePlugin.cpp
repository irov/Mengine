#include "SpinePlugin.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"

#include "SpineScriptEmbedding.h"

#include "ResourceSpineAtlasDefault.h"
#include "ResourceSpineAtlasTexturepacker.h"
#include "ResourceSpineSkeleton.h"

#include "LoaderResourceSpineAtlasDefault.h"
#include "LoaderResourceSpineAtlasTexturepacker.h"
#include "LoaderResourceSpineSkeleton.h"

#include "Kernel/ResourcePrototypeGenerator.h"

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
        SERVICE_WAIT( ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( SpineScriptEmbedding );

            return true;
        } );

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
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( SpineScriptEmbedding );
        }

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
    }
}