#pragma once

#include "Kernel/BaseRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NoneRender
        : public BaseRender
    {
    public:
        NoneRender();
        ~NoneRender() override;

    public:
        void render( const RenderContext * _context ) const override;

    public:
        void renderWithChildren( const RenderContext * _context, bool _external ) const override;
    };
}