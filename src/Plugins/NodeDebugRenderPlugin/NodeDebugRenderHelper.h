#pragma once

#include "Kernel/Polygon.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderContext.h"
#include "Kernel/VectorRenderVertex2D.h"

namespace Mengine
{
    namespace Helper
    {
        void nodeDebugRenderLine( const RenderContext * _context, const VectorRenderVertex2D & _vertices );
        void nodeDebugRenderCircle( const RenderContext * _context, const mt::mat4f & _wm, float _radius, uint32_t _count, uint32_t _color );
        void nodeDebugRenderPolygon( const RenderContext * _context, const mt::mat4f & _wm, const Polygon & _polygon, uint32_t _color );
    };
}