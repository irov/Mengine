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
    void PointDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, Point * _node, bool _hide )
    {
        if( _hide == true )
        {
            return;
        }

        const TransformationInterface * transformation = _node->getTransformation();

        const mt::mat4f & wm = transformation->getWorldMatrix();

        float radius = 2.5f;

        Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 4, 0xFF0000FF, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
}