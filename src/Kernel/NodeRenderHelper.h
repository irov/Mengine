#pragma once

#include "Interface/RenderInterface.h"
#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#include "Kernel/Node.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        void nodeRenderChildren( const NodePtr & _node, const RenderContext * _context, bool _external );
        RenderInterface * getNodeRenderInheritance( Node * _node );
        const RenderViewportInterfacePtr & getRenderViewportInheritance( RenderInterface * _render );
        const RenderCameraInterfacePtr & getRenderCameraInheritance( RenderInterface * _render );
        const RenderScissorInterfacePtr & getRenderScissorInheritance( RenderInterface * _render );
        const RenderTargetInterfacePtr & getRenderTargetInheritance( RenderInterface * _render );
        const RenderViewportInterfacePtr & getNodeRenderViewportInheritance( Node * _node );
        const RenderCameraInterfacePtr & getNodeRenderCameraInheritance( Node * _node );
        const RenderScissorInterfacePtr & getNodeRenderScissorInheritance( Node * _node );
        const RenderTargetInterfacePtr & getNodeRenderTargetInheritance( Node * _node );
    }
}