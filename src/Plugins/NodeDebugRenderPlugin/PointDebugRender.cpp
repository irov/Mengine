#include "PointDebugRender.h"

#include "NodeDebugRenderHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PointDebugRender::PointDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PointDebugRender::~PointDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PointDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, Point * _node )
    {
        const mt::mat4f & wm = _node->getWorldMatrix();

        float radius = 2.5f;

        Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 4, 0xFF0000FF, MENGINE_DOCUMENT_FACTORABLE );
    }
}