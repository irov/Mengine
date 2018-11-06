#include "DebugRenderPlugin.h"

#include "DebugRenderInterface.h"

#include "NodeDebugRenderVisitor.h"

#include "Kernel/FactorableUnique.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( DebugRenderService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DebugRender, Mengine::DebugRenderPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DebugRenderPlugin::DebugRenderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DebugRenderPlugin::~DebugRenderPlugin()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool DebugRenderPlugin::_avaliable()
    {
        return true;
    }    
    //////////////////////////////////////////////////////////////////////////
    bool DebugRenderPlugin::_initialize()
    {
        SERVICE_CREATE( DebugRenderService );

        RenderVisitorPtr nodeRenderVisitor = new FactorableUnique<NodeDebugRenderVisitor>();

        DEBUGRENDER_SERVICE()
            ->addDebugNodeRenderVisitor( nodeRenderVisitor );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugRenderPlugin::_finalize()
    {
        SERVICE_FINALIZE( Mengine::DebugRenderServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugRenderPlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::DebugRenderServiceInterface );
    }
}
