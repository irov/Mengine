#pragma once

#include "Interface/RenderPipelineInterface.h"

#include "Kernel/Polygon.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderContext.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/Color.h"

namespace Mengine
{
    namespace Helper
    {
        void nodeDebugRenderLine( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const VectorRenderVertex2D & _vertices, const DocumentInterfacePtr & _doc );
        void nodeDebugRenderLineByToPoints( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::mat4f & _wm, const mt::vec2f & _startPoint, const mt::vec2f & _finishPoint, ColorValue_ARGB _color, const DocumentInterfacePtr & _doc );
        void nodeDebugRenderCircle( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::mat4f & _wm, float _radius, uint32_t _count, ColorValue_ARGB _color, const DocumentInterfacePtr & _doc );
        void nodeDebugRenderPolygon( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::mat4f & _wm, const Polygon & _polygon, ColorValue_ARGB _color, const DocumentInterfacePtr & _doc );
        void nodeDebugRenderPoint( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::mat4f & _wm, const mt::vec2f & _point, ColorValue_ARGB _color, float _length, const DocumentInterfacePtr & _doc );
    };
}