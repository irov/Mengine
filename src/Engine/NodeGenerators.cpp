#include "NodeGenerators.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

#include "Kernel/Interender.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"

#include "Isometric.h"
#include "Grid2D.h"
#include "Gyroscope.h"
#include "HotSpotBubbles.h"
#include "HotSpotCircle.h"
#include "HotSpotGlobal.h"
#include "HotSpotImage.h"
#include "HotSpotPolygon.h"
#include "HotSpotShape.h"
#include "HotSpotShape.h"
#include "Landscape2D.h"
#include "Line.h"
#include "Meshget.h"
#include "Point.h"
#include "SoundEmitter.h"
#include "TextField.h"
#include "Window.h"
#include "ShapeCircle.h"
#include "ShapePacMan.h"
#include "ShapeQuadFixed.h"
#include "ShapeQuadFlex.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Helper
    {
        NodePtr generateNode( const DocumentPtr & _doc )
        {
            NodePtr node = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Node" ), _doc );

            return node;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_NODE_GENERATE(X)\
namespace Mengine::Helper{\
        X ## Ptr generate ## X( const DocumentPtr & _doc ){\
            X ## Ptr node = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( #X ), _doc );\
            return node;\
        }}
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
MENGINE_NODE_GENERATE( HotSpotShape )
MENGINE_NODE_GENERATE( Landscape2D )
MENGINE_NODE_GENERATE( Line )
MENGINE_NODE_GENERATE( Meshget )
MENGINE_NODE_GENERATE( Point )
MENGINE_NODE_GENERATE( SoundEmitter )
MENGINE_NODE_GENERATE( TextField )
MENGINE_NODE_GENERATE( Window )
MENGINE_NODE_GENERATE( ShapeCircle )
MENGINE_NODE_GENERATE( ShapePacMan )
MENGINE_NODE_GENERATE( ShapeQuadFixed )
MENGINE_NODE_GENERATE( ShapeQuadFlex )
//////////////////////////////////////////////////////////////////////////
#undef MENGINE_NODE_GENERATE
//////////////////////////////////////////////////////////////////////////