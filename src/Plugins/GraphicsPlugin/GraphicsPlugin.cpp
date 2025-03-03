#include "GraphicsPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Graphics.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Graphics, Mengine::GraphicsPlugin );
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
        if( Helper::addNodePrototype<Graphics, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPlugin::_finalizePlugin()
    {
        Helper::removeNodePrototype<Graphics>();
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "gp" );
    }
    //////////////////////////////////////////////////////////////////////////
}