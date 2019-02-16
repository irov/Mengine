#include "OzzAnimationPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

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
    bool OzzAnimationPlugin::_initializePlugin()
    {
        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzSkeleton"_c, new FactorableUnique<DefaultPrototypeGenerator<ResourceOzzSkeleton, 16> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzMesh"_c, new FactorableUnique<DefaultPrototypeGenerator<ResourceOzzMesh, 16> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceOzzAnimation"_c, new FactorableUnique < DefaultPrototypeGenerator<ResourceOzzAnimation, 16> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Sampler"_c, "SamplerOzzAnimation"_c, new FactorableUnique < DefaultPrototypeGenerator<SamplerOzzAnimation, 16> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Node"_c, "NodeOzzAnimation"_c, new FactorableUnique < NodePrototypeGenerator<NodeOzzAnimation, 16> > ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzAnimationPlugin::_finalizePlugin()
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