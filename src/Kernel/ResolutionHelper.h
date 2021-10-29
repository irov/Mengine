#pragma once

#include "Config/Config.h"

#include "math/vec2.h"

namespace Mengine
{
    namespace Helper
    {
        void adaptScreenPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint );
        void adaptScreenDeltha( const mt::vec2f & _screenDeltha, mt::vec2f * const _adaptScreenDeltha );
        void adaptWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint );
    }
}
