#pragma once

#include "BaseDebugRender.h"

#include "Kernel/Arrow.h"

namespace Mengine
{
    class ArrowDebugRender
        : public BaseDebugRenderT<Arrow>
    {
    public:
        ArrowDebugRender();
        ~ArrowDebugRender() override;

    protected:
        void _render( const RenderContext * _context, Arrow * _node ) override;
    };
}