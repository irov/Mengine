#pragma once

#include "Kernel/RenderContext.h"

namespace Mengine
{
    namespace Helper
    {
        bool equalRenderContext( const RenderContext * _left, const RenderContext * _right );
        void clearRenderContext( RenderContext * const _renderContext );
    }
}