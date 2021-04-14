#pragma once

#include "BaseDebugRender.h"

#include "Engine/HotSpotPolygon.h"

namespace Mengine
{
    class HotSpotPolygonDebugRender
        : public BaseDebugRenderT<HotSpotPolygon>
    {
    public:
        HotSpotPolygonDebugRender();
        ~HotSpotPolygonDebugRender() override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const HotSpotPolygon * _node, bool _hide ) override;
    };
}