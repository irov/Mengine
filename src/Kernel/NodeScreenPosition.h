#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
    namespace Helper
    {
        void getNodeScreenPosition( const Node * _node, mt::vec2f * const _screenPosition );
        void setNodeScreenPosition( Node * _node, const mt::vec2f & _screenPosition, float _deep );
    }
}