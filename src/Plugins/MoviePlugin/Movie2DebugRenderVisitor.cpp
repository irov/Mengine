#include "Movie2DebugRenderVisitor.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/RenderVertex2D.h"

#include "Plugins/DebugRenderPlugin/DebugRenderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2DebugRenderVisitor::Movie2DebugRenderVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2DebugRenderVisitor::~Movie2DebugRenderVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2DebugRenderVisitor::accept( Movie2 * _node )
    {
        _node->foreachRenderSlots( m_context, []( Node * _node, const RenderContext * _context )
        {
            DEBUGRENDER_SERVICE()
                ->renderDebugNode( _node, _context, true );
        } );
    }
}