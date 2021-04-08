#pragma once

#include "BaseDebugRender.h"

#include "Engine/HotSpotSurface.h"

namespace Mengine
{
    class HotSpotSurfaceDebugRender
        : public BaseDebugRenderT<HotSpotSurface>
    {
    public:
        HotSpotSurfaceDebugRender();
        ~HotSpotSurfaceDebugRender() override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const HotSpotSurface * _surface, bool _hide ) override;
    };
}