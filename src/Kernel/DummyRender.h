#pragma once

#include "Kernel/BaseRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DummyRender
        : public BaseRender
    {
    public:
        DummyRender();
        ~DummyRender() override;

    public:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    public:
        void renderWithChildren( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external ) const override;
    };
}