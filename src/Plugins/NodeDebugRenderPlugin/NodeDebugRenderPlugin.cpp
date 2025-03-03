#include "NodeDebugRenderPlugin.h"

#include "NodeDebugRenderServiceInterface.h"

#include "PointDebugRender.h"
#include "HotSpotCircleDebugRender.h"
#include "HotSpotImageDebugRender.h"
#include "HotSpotPolygonDebugRender.h"
#include "HotSpotSurfaceDebugRender.h"
#include "TextFieldDebugRender.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( NodeDebugRenderService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( NodeDebugRender, Mengine::NodeDebugRenderPlugin );
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
            ->addNodeDebugRender( Point::getFactorableType(), Helper::makeFactorableUnique<PointDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( HotSpotCircle::getFactorableType(), Helper::makeFactorableUnique<HotSpotCircleDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( HotSpotImage::getFactorableType(), Helper::makeFactorableUnique<HotSpotImageDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( HotSpotPolygon::getFactorableType(), Helper::makeFactorableUnique<HotSpotPolygonDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( HotSpotSurface::getFactorableType(), Helper::makeFactorableUnique<HotSpotSurfaceDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

        NODEDEBUGRENDER_SERVICE()
            ->addNodeDebugRender( TextField::getFactorableType(), Helper::makeFactorableUnique<TextFieldDebugRender>( MENGINE_DOCUMENT_FACTORABLE ) );

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
            ->removeNodeDebugRender( Point::getFactorableType() );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( HotSpotCircle::getFactorableType() );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( HotSpotImage::getFactorableType() );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( HotSpotPolygon::getFactorableType() );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( HotSpotSurface::getFactorableType() );

        NODEDEBUGRENDER_SERVICE()
            ->removeNodeDebugRender( TextField::getFactorableType() );

        SERVICE_FINALIZE( NodeDebugRenderService );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( NodeDebugRenderService );
    }
    //////////////////////////////////////////////////////////////////////////
}
