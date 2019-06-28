#include "OzzAnimationPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "ResourceOzzMesh.h"
#include "ResourceOzzSkeleton.h"
#include "ResourceOzzAnimation.h"

#include "NodeOzzAnimation.h"
#include "OzzScriptEmbedding.h"

#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/Document.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( OzzAnimation, Mengine::OzzAnimationPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OzzAnimationPlugin::OzzAnimationPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OzzAnimationPlugin::~OzzAnimationPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzAnimationPlugin::_initializePlugin()
    {
        SERVICE_WAIT( ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( OzzScriptEmbedding );

            return true;
        } );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzSkeleton"_c, Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzSkeleton, 16>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzMesh"_c, Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzMesh, 16>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzAnimation"_c, Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzAnimation, 16>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Sampler"_c, "SamplerOzzAnimation"_c, Helper::makeFactorableUnique<ScriptablePrototypeGenerator<SamplerOzzAnimation, 16>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Node"_c, "NodeOzzAnimation"_c, Helper::makeFactorableUnique<NodePrototypeGenerator<NodeOzzAnimation, 16>>() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzAnimationPlugin::_finalizePlugin()
    {
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( OzzScriptEmbedding );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( "Resource"_c, "ResourceOzzSkeleton"_c );

        PROTOTYPE_SERVICE()
            ->removePrototype( "Resource"_c, "ResourceOzzMesh"_c );

        PROTOTYPE_SERVICE()
            ->removePrototype( "Resource"_c, "ResourceOzzAnimation"_c );

        PROTOTYPE_SERVICE()
            ->removePrototype( "Sampler"_c, "SamplerOzzAnimation"_c );

        PROTOTYPE_SERVICE()
            ->removePrototype( "Node"_c, "NodeOzzAnimation"_c );
    }
}