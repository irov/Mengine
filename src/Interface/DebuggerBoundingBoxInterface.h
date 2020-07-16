#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Node.h"

#include "math/box2.h"

namespace Mengine
{
    class DebuggerBoundingBoxInterface
        : public Mixin
    {
    public:
        virtual bool getBoundingBox( const NodePtr & _node, mt::box2f * const _bb ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DebuggerBoundingBoxInterface> DebuggerBoundingBoxInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
