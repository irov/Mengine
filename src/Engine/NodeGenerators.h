#pragma once

#include "Kernel/Node.h"
#include "Kernel/Document.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Helper
    {
        NodePtr generateNode( const DocumentPtr & _doc );
    }
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_NODE_GENERATE(X)\
    namespace Mengine{typedef IntrusiveNodePtr<class X> X ## Ptr;}\
    namespace Mengine::Helper{X ## Ptr generate ## X( const DocumentPtr & _doc );}
//////////////////////////////////////////////////////////////////////////
MENGINE_NODE_GENERATE( Interender )
MENGINE_NODE_GENERATE( Isometric )
MENGINE_NODE_GENERATE( RenderCameraOrthogonal )
MENGINE_NODE_GENERATE( RenderCameraOrthogonalTarget )
MENGINE_NODE_GENERATE( RenderCameraProjection )
MENGINE_NODE_GENERATE( RenderViewport )
MENGINE_NODE_GENERATE( RenderScissor )
MENGINE_NODE_GENERATE( Grid2D )
MENGINE_NODE_GENERATE( Gyroscope )
MENGINE_NODE_GENERATE( HotSpotBubbles )
MENGINE_NODE_GENERATE( HotSpotCircle )
MENGINE_NODE_GENERATE( HotSpotGlobal )
MENGINE_NODE_GENERATE( HotSpotImage )
MENGINE_NODE_GENERATE( HotSpotPolygon )
MENGINE_NODE_GENERATE( HotSpotResourceShape )
MENGINE_NODE_GENERATE( HotSpotSurface )
MENGINE_NODE_GENERATE( Landscape2D )
MENGINE_NODE_GENERATE( Line )
MENGINE_NODE_GENERATE( Meshget )
MENGINE_NODE_GENERATE( Point )
MENGINE_NODE_GENERATE( SoundEmitter )
MENGINE_NODE_GENERATE( TextField )
MENGINE_NODE_GENERATE( Window )
MENGINE_NODE_GENERATE( ShapeCircle )
MENGINE_NODE_GENERATE( ShapePacMan )
MENGINE_NODE_GENERATE( ShapeQuadSize )
MENGINE_NODE_GENERATE( ShapeQuadFixed )
MENGINE_NODE_GENERATE( ShapeQuadFlex )
//////////////////////////////////////////////////////////////////////////
#undef MENGINE_NODE_GENERATE
//////////////////////////////////////////////////////////////////////////