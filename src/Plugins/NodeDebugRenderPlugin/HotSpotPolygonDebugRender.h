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
        void _render( const RenderContext * _context, HotSpotPolygon * _node ) override;
    };
}