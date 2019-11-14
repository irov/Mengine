#pragma once

#include "Kernel/BaseRender.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class Layer2D;

    class Layer2DRenderTarget
        : public BaseRender
        , public Factorable
    {
    public:
        explicit Layer2DRenderTarget( Layer2D * _layer );
        ~Layer2DRenderTarget() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        Layer2D * m_layer;
    };
}