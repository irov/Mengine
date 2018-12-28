#include "NodeDebugRenderPlugin.h"

#include "Interface/StringizeServiceInterface.h"

#include "NodeDebugRenderServiceInterface.h"

#include "ArrowDebugRender.h"
#include "PointDebugRender.h"
#include "HotSpotCircleDebugRender.h"
#include "HotSpotImageDebugRender.h"
#include "HotSpotPolygonDebugRender.h"
#include "TextFieldDebugRender.h"

#include "Kernel/FactorableUnique.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( NodeDebugRenderService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( NodeDebugRender, Mengine::NodeDebugRenderPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebugRenderPlugin::NodeDebugRenderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebugRenderPlugin::~NodeDebugRenderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebugRenderPlugin::_initializePlugin()
    {
        SERVICE_CREATE( NodeDebugRenderService );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Arrow" ), new FactorableUnique<ArrowDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Point" ), new FactorableUnique<PointDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotCircle" ), new FactorableUnique<HotSpotCircleDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotImage" ), new FactorableUnique<HotSpotImageDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), new FactorableUnique<HotSpotPolygonDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "TextField" ), new FactorableUnique<TextFieldDebugRender>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderPlugin::_finalizePlugin()
    {
        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "Arrow" ) );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "Point" ) );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotCircle" ) );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotImage" ) );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ) );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "TextField" ) );

        SERVICE_FINALIZE( Mengine::NodeDebugRenderServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderPlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::NodeDebugRenderServiceInterface );
    }
}
