#include "Movie2DebugRender.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/RenderVertex2D.h"

#include "Plugins/NodeDebugRenderPlugin/NodeDebugRenderServiceInterface.h"

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
            NODEDEBUGRENDER_SERVICE()
                ->renderDebugNode( NodePtr::from( _node ), _renderPipeline, _context, true, _hide );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
}