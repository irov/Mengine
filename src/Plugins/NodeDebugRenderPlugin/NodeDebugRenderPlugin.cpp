#include "NodeDebugRenderPlugin.h"

#include "NodeDebugRenderServiceInterface.h"

#include "ArrowDebugRender.h"
#include "PointDebugRender.h"
#include "HotSpotCircleDebugRender.h"
#include "HotSpotImageDebugRender.h"
#include "HotSpotPolygonDebugRender.h"
#include "HotSpotSurfaceDebugRender.h"
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
        if( SERVICE_CREATE( NodeDebugRenderService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( SERVICE_IS_AVAILABLE( NodeDebugRenderServiceInterface ) == false )
        {
            return true;
        }

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Arrow" ), Helper::makeFactorableUnique<ArrowDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Point" ), Helper::makeFactorableUnique<PointDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotCircle" ), Helper::makeFactorableUnique<HotSpotCircleDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotImage" ), Helper::makeFactorableUnique<HotSpotImageDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), Helper::makeFactorableUnique<HotSpotPolygonDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotSurface" ), Helper::makeFactorableUnique<HotSpotSurfaceDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );        

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "TextField" ), Helper::makeFactorableUnique<TextFieldDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderPlugin::_finalizePlugin()
    {
        if( SERVICE_IS_INITIALIZE( NodeDebugRenderServiceInterface ) == false )
        {
            return;
        }

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
            ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "HotSpotSurface" ) );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "TextField" ) );

        SERVICE_FINALIZE( NodeDebugRenderService );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( NodeDebugRenderService );
    }
    //////////////////////////////////////////////////////////////////////////
}
