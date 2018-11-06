#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/RenderInterface.h"

#include "Kernel/Node.h"
#include "Kernel/RenderVisitor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DebugRenderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DebugRenderService" );

    public:
        virtual void renderDebugNode( const NodePtr & _node, const RenderContext * _context, bool _external ) = 0;
        
    public:
        virtual void addDebugNodeRenderVisitor( const RenderVisitorPtr & _renderVisitor ) = 0;
        virtual void removeDebugNodeRenderVisitor( const RenderVisitorPtr & _renderVisitor ) = 0;

    public:
        virtual void updateDebugInfo( const UpdateContext * _context ) = 0;
        virtual void renderDebugInfo( const RenderContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define DEBUGRENDER_SERVICE()\
    ((Mengine::DebugRenderServiceInterface *)SERVICE_GET(Mengine::DebugRenderServiceInterface))
}