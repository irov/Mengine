#include "DebugRenderPlugin.h"

#include "DebugRenderInterface.h"

#include "NodeDebugRenderVisitor.h"

#include "Kernel/FactorableUnique.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( DebugRender );
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
        SERVICE_CREATE( DebugRender );

        RenderVisitorPtr nodeRenderVisitor = new FactorableUnique<NodeDebugRenderVisitor>();

        DEBUGRENDER_SERVICE()
            ->addRenderVisitor( nodeRenderVisitor );

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
