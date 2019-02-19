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
    void Movie2DebugRender::_render( const RenderContext * _context, Movie2 * _node )
    {
        _node->foreachRenderSlots( _context, []( Node * _node, const RenderContext * _context )
        {
            NODEDEBUGRENDER_SERVICE()
                ->renderDebugNode( NodePtr( _node ), _context, true );
        } );
    }
}