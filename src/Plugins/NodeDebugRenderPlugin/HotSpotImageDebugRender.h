#pragma once

#include "BaseDebugRender.h"

#include "Engine/HotSpotImage.h"

namespace Mengine
{
    class HotSpotImageDebugRender
        : public BaseDebugRenderT<HotSpotImage>
    {
    public:
        HotSpotImageDebugRender();
        ~HotSpotImageDebugRender() override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, HotSpotImage * _node ) override;
    };
}