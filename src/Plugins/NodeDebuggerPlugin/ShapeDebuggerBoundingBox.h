#pragma once

#include "NodeDebuggerBoundingBoxInterface.h"

#include "Kernel/Shape.h"

namespace Mengine
{
    class ShapeDebuggerBoundingBox
        : public BaseDebuggerBoundingBox<Shape>
        , public Factorable
    {
    public:
        bool _updateBoundingBox( const ShapePtr & _node, mt::box2f * _bb ) override;
    };
}
