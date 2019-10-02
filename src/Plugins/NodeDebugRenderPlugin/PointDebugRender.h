#pragma once

#include "BaseDebugRender.h"

#include "Engine/Point.h"

namespace Mengine
{
    class PointDebugRender
        : public BaseDebugRenderT<Point>
    {
    public:
        PointDebugRender();
        ~PointDebugRender() override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, Point * _node ) override;
    };
}