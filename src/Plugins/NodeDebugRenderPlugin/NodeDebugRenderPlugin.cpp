#include "NodeDebugRenderPlugin.h"

#include "NodeDebugRenderServiceInterface.h"

#include "ArrowDebugRender.h"
#include "PointDebugRender.h"
#include "HotSpotCircleDebugRender.h"
#include "HotSpotImageDebugRender.h"
#include "HotSpotPolygonDebugRender.h"
#include "TextFieldDebugRender.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"

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
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Arrow" ), Helper::makeFactorableUnique<ArrowDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Point" ), Helper::makeFactorableUnique<PointDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotCircle" ), Helper::makeFactorableUnique<HotSpotCircleDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotImage" ), Helper::makeFactorableUnique<HotSpotImageDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), Helper::makeFactorableUnique<HotSpotPolygonDebugRender>() );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "TextField" ), Helper::makeFactorableUnique<TextFieldDebugRender>() );

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

        SERVICE_FINALIZE( NodeDebugRenderService );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( NodeDebugRenderService );
    }
}
