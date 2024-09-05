#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ArrowInterface.h"

#include "NodeDebugRenderInterface.h"

#include "Kernel/Node.h"
#include "Kernel/ConstString.h"
#include "Kernel/RenderContext.h"
#include "Kernel/UpdateContext.h"

namespace Mengine
{
    class NodeDebugRenderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "NodeDebugRenderService" );

    public:
        virtual void renderDebugArrow( const ArrowInterfacePtr & _arrow, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external, bool _hide ) = 0;
        virtual void renderDebugNode( const NodePtr & _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external, bool _hide ) = 0;

    public:
        virtual void addNodeDebugRender( const ConstString & _type, const NodeDebugRenderInterfacePtr & _renderVisitor ) = 0;
        virtual void removeNodeDebugRender( const ConstString & _type ) = 0;

    public:
        virtual void updateDebugInfo( const UpdateContext * _context ) = 0;
        virtual void renderDebugInfo( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define NODEDEBUGRENDER_SERVICE()\
    ((Mengine::NodeDebugRenderServiceInterface *)SERVICE_GET(Mengine::NodeDebugRenderServiceInterface))
//////////////////////////////////////////////////////////////////////////