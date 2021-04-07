#pragma once

#include "Interface/RenderPipelineInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/RenderContext.h"
#include "Kernel/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeDebugRenderInterface
        : public Mixin
    {
    public:
        virtual void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const Node * _node, bool _hide ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeDebugRenderInterface> NodeDebugRenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}