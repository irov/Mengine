#pragma once

#include "Config/Config.h"

#include "math/vec2.h"

namespace Mengine
{
    namespace Helper
    {
        void adaptScreenPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint );
        void adaptScreenDelta( const mt::vec2f & _screenDelta, mt::vec2f * const _adaptScreenDelta );
        void adaptWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint );
    }
}
