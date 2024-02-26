#pragma once

#include "Kernel/RenderContext.h"
#include "Kernel/Resolution.h"

#include "math/vec2.h"
#include "math/box2.h"

namespace Mengine
{
    namespace Helper
    {
        void screenToWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint );
        void screenToWorldDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * const _worldDelta );
        void worldToScreenPosition( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _worldPosition, mt::vec2f * const _screenPosition );
        void worldToScreenDelta( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _worldDelta, mt::vec2f * const _screenDelta );
        void worldToScreenBox( const RenderContext * _context, const Resolution & _contentResolution, const mt::box2f & _worldBox, mt::box2f * const _screenBox );
    }
}
