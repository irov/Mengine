#include "GraphicsPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Graphics.h"
#include "GraphicsPrototypeGenerator.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Graphics, Mengine::GraphicsPlugin );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( GraphicsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GraphicsPlugin::GraphicsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsPlugin::~GraphicsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( GraphicsService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        gp_graphics_t * graphics = GRAPHICS_SERVICE()
            ->getGraphics();

        GraphicsPrototypeGeneratorPtr prototypeGenerator = Helper::makeFactorableUnique<GraphicsPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        prototypeGenerator->setGraphics( graphics );

        const ConstString & nodeFactorableType = Node::getFactorableType();
        const ConstString & graphicsFactorableType = Graphics::getFactorableType();

        if( PROTOTYPE_SERVICE()
            ->addPrototype( nodeFactorableType, graphicsFactorableType, prototypeGenerator ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPlugin::_finalizePlugin()
    {
        Helper::removeNodePrototype<Graphics>();

        SERVICE_FINALIZE( GraphicsService );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( GraphicsService );

        MENGINE_ASSERTION_ALLOCATOR( "gp" );
    }
    //////////////////////////////////////////////////////////////////////////
}