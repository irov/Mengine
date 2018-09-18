#pragma once

#include "Interface/RenderInterface.h"
#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#include "Kernel/RenderVisitor.h"

namespace Mengine
{
    typedef IntrusivePtr<class Node> NodePtr;

    namespace Helper
    {
        void nodeRenderChildren( const NodePtr & _node, const RenderContext * _context );
        void nodeRenderChildrenVisitor( const NodePtr & _node, const RenderVisitorPtr & _visitor, const RenderContext * _context );
        RenderInterfacePtr getRenderInheritance( const Node * _node );
        const RenderViewportInterfacePtr & getRenderViewportInheritance( const Node * _node );
        const RenderCameraInterfacePtr & getRenderCameraInheritance( const Node * _node );
        const RenderScissorInterfacePtr & getRenderScissorInheritance( const Node * _node );
        const RenderTargetInterfacePtr & getRenderTargetInheritance( const Node * _node );
    }
}