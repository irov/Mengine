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
        virtual void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, Node * _node ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeDebugRenderInterface> NodeDebugRenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}