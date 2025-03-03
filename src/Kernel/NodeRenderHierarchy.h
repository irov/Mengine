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
        //////////////////////////////////////////////////////////////////////////
        void nodeRenderChildren( const Node * _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external );
        bool hasNodeRenderInheritance( const Node * _node );
        RenderInterface * getNodeRenderInheritance( Node * _node, Node ** const _nodeRender );
        //////////////////////////////////////////////////////////////////////////
        const RenderResolutionInterface * getRenderResolutionInheritance( const RenderInterface * _render );
        const RenderViewportInterface * getRenderViewportInheritance( const RenderInterface * _render );
        const RenderCameraInterface * getRenderCameraInheritance( const RenderInterface * _render );
        const RenderTransformationInterface * getRenderTransformationInheritance( const RenderInterface * _render );
        const RenderScissorInterface * getRenderScissorInheritance( const RenderInterface * _render );
        const RenderTargetInterface * getRenderTargetInheritance( const RenderInterface * _render );
        ZGroupType getRenderZGroupInheritance( const RenderInterface * _render );
        ZIndexType getRenderZIndexInheritance( const RenderInterface * _render );
        //////////////////////////////////////////////////////////////////////////
        const RenderResolutionInterface * getNodeRenderResolutionInheritance( const Node * _node );
        const RenderViewportInterface * getNodeRenderViewportInheritance( const Node * _node );
        const RenderCameraInterface * getNodeRenderCameraInheritance( const Node * _node );
        const RenderTransformationInterface * getNodeRenderTransformationInheritance( const Node * _node );
        const RenderScissorInterface * getNodeRenderScissorInheritance( const Node * _node );
        const RenderTargetInterface * getNodeRenderTargetInheritance( const Node * _node );
        ZGroupType getNodeRenderZGroupInheritance( const Node * _node );
        ZIndexType getNodeRenderZIndexInheritance( const Node * _node );
        //////////////////////////////////////////////////////////////////////////
        void fillNodeRenderContextInheritance( const NodePtr & _node, RenderContext * const _context );
        //////////////////////////////////////////////////////////////////////////
    }
}