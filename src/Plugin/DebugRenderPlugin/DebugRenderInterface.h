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
        virtual void render( const NodePtr & _node, const RenderContext * _context ) = 0;

    public:
        virtual void addRenderVisitor( const RenderVisitorPtr & _renderVisitor ) = 0;
        virtual void removeRenderVisitor( const RenderVisitorPtr & _renderVisitor ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define DEBUGRENDER_SERVICE()\
    ((Mengine::DebugRenderServiceInterface *)SERVICE_GET(Mengine::DebugRenderServiceInterface))
}