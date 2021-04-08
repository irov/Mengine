#pragma once

#include "BaseDebugRender.h"

#include "Engine/HotSpotCircle.h"

namespace Mengine
{
    class HotSpotCircleDebugRender
        : public BaseDebugRenderT<HotSpotCircle>
    {
    public:
        HotSpotCircleDebugRender();
        ~HotSpotCircleDebugRender() override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const HotSpotCircle * _node, bool _hide ) override;
    };
}