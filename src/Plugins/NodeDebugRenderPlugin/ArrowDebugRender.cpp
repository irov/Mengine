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
    void ArrowDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, Arrow * _node, bool _hide )
    {
        MENGINE_UNUSED( _hide );

        const TransformationInterface * transformation = _node->getTransformation();

        const mt::mat4f & wm = transformation->getWorldMatrix();

        EArrowType arrowType = _node->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                float radius = 2.5f;

                Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 4, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
            }break;
        case EAT_RADIUS:
            {
                float radius = _node->getRadius();

                Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 32, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
            }break;
        case EAT_POLYGON:
            {
                const Polygon & polygon = _node->getPolygon();

                Helper::nodeDebugRenderPolygon( _renderPipeline, _context, wm, polygon, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
            }break;
        }
    }
}