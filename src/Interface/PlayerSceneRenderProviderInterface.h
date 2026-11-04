#pragma once

#include "Interface/RenderPipelineInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Node.h"
#include "Kernel/RenderContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PlayerSceneRenderProviderInterface
        : public Mixin
    {
    public:
        virtual void renderScene( const NodePtr & _scene, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) = 0;
        virtual void renderArrow( const NodePtr & _arrow, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PlayerSceneRenderProviderInterface> PlayerSceneRenderProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
