#pragma once

#include "Interface/RenderInterface.h"
#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#include "Kernel/RenderVisitor.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef const Lambda<void(const RenderInterfacePtr &)> LambdaNodeRenderCloseChildren;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        void nodeRenderChildren( const NodePtr & _node, const RenderContext * _context );
        void nodeRenderChildrenExternal( const NodePtr & _node, const RenderContext * _context );
        void nodeRenderChildrenVisitor( const NodePtr & _node, const RenderVisitorPtr & _visitor, const RenderContext * _context );        
        RenderInterface * getNodeRenderInheritance( Node * _node );
        void visitNodeRenderCloseChildren( const Node * _node, const LambdaNodeRenderCloseChildren & _lambda );
        const RenderViewportInterfacePtr & getNodeRenderViewportInheritance( Node * _node );
        const RenderCameraInterfacePtr & getNodeRenderCameraInheritance( Node * _node );
        const RenderScissorInterfacePtr & getNodeRenderScissorInheritance( Node * _node );
        const RenderTargetInterfacePtr & getNodeRenderTargetInheritance( Node * _node );
    }
}