#pragma once

#include "Interface/RenderInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/RenderOrderInterface.h"
#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#include "Kernel/Node.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void nodeRenderChildren( const Node * _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external );
        bool hasNodeRenderInheritance( const Node * _node );
        RenderInterface * getNodeRenderInheritance( Node * _node, Node ** const _nodeRender );
        //////////////////////////////////////////////////////////////////////////
        const RenderOrderInterfacePtr & getRenderOrederInheritance( const RenderInterface * _render );
        const RenderViewportInterfacePtr & getRenderViewportInheritance( const RenderInterface * _render );
        const RenderCameraInterfacePtr & getRenderCameraInheritance( const RenderInterface * _render );
        const RenderTransformationInterfacePtr & getRenderTransformationInheritance( const RenderInterface * _render );
        const RenderScissorInterfacePtr & getRenderScissorInheritance( const RenderInterface * _render );
        const RenderTargetInterfacePtr & getRenderTargetInheritance( const RenderInterface * _render );
        //////////////////////////////////////////////////////////////////////////
        void fillNodeRenderContext( const NodePtr & _node, RenderContext * const _context );
        //////////////////////////////////////////////////////////////////////////
        const RenderOrderInterfacePtr & getNodeRenderOrderInheritance( const Node * _node );
        const RenderViewportInterfacePtr & getNodeRenderViewportInheritance( const Node * _node );
        const RenderCameraInterfacePtr & getNodeRenderCameraInheritance( const Node * _node );
        const RenderTransformationInterfacePtr & getNodeRenderTransformationInheritance( const Node * _node );
        const RenderScissorInterfacePtr & getNodeRenderScissorInheritance( const Node * _node );
        const RenderTargetInterfacePtr & getNodeRenderTargetInheritance( const Node * _node );
        //////////////////////////////////////////////////////////////////////////
    }
}