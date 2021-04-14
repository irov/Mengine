#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/Renderable.h"
#include "Kernel/BaseRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Layer2D;
    //////////////////////////////////////////////////////////////////////////
    class Layer2DRenderTarget
        : public Factorable
        , public Renderable
        , protected BaseRender
    {
        DECLARE_FACTORABLE( Layer2DRenderTarget );
        DECLARE_RENDERABLE();

    public:
        explicit Layer2DRenderTarget( Layer2D * _layer );
        ~Layer2DRenderTarget() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        Layer2D * m_layer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Layer2DRenderTarget> Layer2DRenderTargetPtr;
    //////////////////////////////////////////////////////////////////////////
}