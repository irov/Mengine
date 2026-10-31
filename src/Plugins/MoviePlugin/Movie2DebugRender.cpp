#include "Movie2DebugRender.h"

#include "Interface/RenderServiceInterface.h"

#include "Plugins/NodeDebugRenderPlugin/NodeDebugRenderServiceInterface.h"

#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2DebugRender::Movie2DebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2DebugRender::~Movie2DebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2DebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const Movie2 * _node, bool _hide )
    {
        _node->foreachRenderSlots( _renderPipeline, _context, [_hide]( Node * _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
        {
            NodePtr node = NodePtr::from( _node );

            NODEDEBUGRENDER_SERVICE()
                ->renderDebugNode( node, _renderPipeline, _context, true, _hide );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
}
