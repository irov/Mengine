#pragma once

#include "Interface/RenderInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#include "Kernel/Node.h"

namespace Mengine
{
    namespace Helper
    {
        void nodeRenderChildren( const Node * _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external );
        bool hasNodeRenderInheritance( Node * _node );
        RenderInterface * getNodeRenderInheritance( Node * _node, Node ** _nodeRender );
        const RenderViewportInterfacePtr & getRenderViewportInheritance( RenderInterface * _render );
        const RenderCameraInterfacePtr & getRenderCameraInheritance( RenderInterface * _render );
        const RenderTransformationInterfacePtr & getRenderTransformationInheritance( RenderInterface * _render );
        const RenderScissorInterfacePtr & getRenderScissorInheritance( RenderInterface * _render );
        const RenderTargetInterfacePtr & getRenderTargetInheritance( RenderInterface * _render );
        void getNodeRenderContext( const NodePtr & _node, RenderContext * _context );
        const RenderViewportInterfacePtr & getNodeRenderViewportInheritance( Node * _node );
        const RenderCameraInterfacePtr & getNodeRenderCameraInheritance( Node * _node );
        const RenderTransformationInterfacePtr & getNodeRenderTransformationInheritance( Node * _node );
        const RenderScissorInterfacePtr & getNodeRenderScissorInheritance( Node * _node );
        const RenderTargetInterfacePtr & getNodeRenderTargetInheritance( Node * _node );
    }
}