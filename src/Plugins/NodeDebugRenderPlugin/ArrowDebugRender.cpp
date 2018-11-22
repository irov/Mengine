#include "ArrowDebugRender.h"

#include "NodeDebugRenderHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArrowDebugRender::ArrowDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArrowDebugRender::~ArrowDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowDebugRender::_render( const RenderContext * _context, Arrow * _node )
    {
        const mt::mat4f & wm = _node->getWorldMatrix();

        EArrowType arrowType = _node->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                float radius = 2.5f;

                Helper::nodeDebugRenderCircle( _context, wm, radius, 4, 0x8080FFFF );
            }break;
        case EAT_RADIUS:
            {
                float radius = _node->getRadius();

                Helper::nodeDebugRenderCircle( _context, wm, radius, 32, 0x8080FFFF );
            }break;
        case EAT_POLYGON:
            {
                const Polygon & polygon = _node->getPolygon();

                Helper::nodeDebugRenderPolygon( _context, wm, polygon, 0x8080FFFF );
            }break;
        }
    }
}