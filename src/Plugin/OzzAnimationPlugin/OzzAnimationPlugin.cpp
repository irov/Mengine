#include "OzzAnimationPlugin.h"

#include "Interface/StringizeInterface.h"

#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"

#include "ResourceOzzMesh.h"
#include "ResourceOzzSkeleton.h"
#include "ResourceOzzAnimation.h"

#include "NodeOzzAnimation.h"

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
    bool OzzAnimationPlugin::_initialize()
    {
        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzSkeleton"_c, new DefaultPrototypeGenerator<ResourceOzzSkeleton, 16> ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzMesh"_c, new DefaultPrototypeGenerator<ResourceOzzMesh, 16> ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzAnimation"_c, new DefaultPrototypeGenerator<ResourceOzzAnimation, 16> ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Sampler"_c, "SamplerOzzAnimation"_c, new DefaultPrototypeGenerator<SamplerOzzAnimation, 16> ) == false )
        {
            return false;
        }        

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Node"_c, "NodeOzzAnimation"_c, new NodePrototypeGenerator<NodeOzzAnimation, 16> ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzAnimationPlugin::_finalize()
    {
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