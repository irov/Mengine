#pragma once

#include "Kernel/Node.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeDebugRenderInterface
        : public Mixin
    {
    public:
        virtual void render( const RenderContext * _context, Node * _node ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeDebugRenderInterface> NodeDebugRenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}