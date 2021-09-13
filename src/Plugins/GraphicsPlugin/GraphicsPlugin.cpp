#include "GraphicsPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Graphics.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"

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
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Graphics, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "gp" );
    }
    //////////////////////////////////////////////////////////////////////////
}