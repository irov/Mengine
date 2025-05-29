#pragma once

#include "Kernel/Node.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    namespace Helper
    {
        void getNodeScreenPosition( const Node * _node, const mt::mat4f & _offsetMatrix, mt::vec2f * const _screenPosition );
        void setNodeScreenPosition( Node * _node, const mt::vec2f & _screenPosition, float _deep );
    }
}