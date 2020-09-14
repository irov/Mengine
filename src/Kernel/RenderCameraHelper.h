#pragma once

#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderViewportInterface.h"

#include "Kernel/Resolution.h"

#include "math/vec2.h"
#include "math/box2.h"

namespace Mengine
{
    namespace Helper
    {
        void screenToWorldPosition( const RenderCameraInterfacePtr & _renderCamera, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint );
        void screenToWorldDelta( const RenderCameraInterfacePtr & _renderCamera, const mt::vec2f & _screenDeltha, mt::vec2f * const _worldDeltha );
        void worldToScreenPosition( const RenderCameraInterfacePtr & _renderCamera, const RenderViewportInterfacePtr & _renderViewport, const Resolution & _contentResolution, const mt::vec2f & _worldPosition, mt::vec2f * const _screenPosition );
        void worldToScreenDelta( const RenderCameraInterfacePtr & _renderCamera, const RenderViewportInterfacePtr & _renderViewport, const Resolution & _contentResolution, const mt::vec2f & _worldDelta, mt::vec2f * const _screenDelta );
        void worldToScreenBox( const RenderCameraInterfacePtr & _renderCamera, const RenderViewportInterfacePtr & _renderViewport, const Resolution & _contentResolution, const mt::box2f & _worldBox, mt::box2f * const _screenBox );
    }
}
