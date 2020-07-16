#pragma once

#include "Engine/HotSpotPolygon.h"

#include "Kernel/BaseDebuggerBoundingBox.h"

namespace Mengine
{
    class HotSpotPolygonDebuggerBoundingBox
        : public BaseDebuggerBoundingBox<HotSpotPolygonPtr>
        , public Factorable
    {
    public:
        HotSpotPolygonDebuggerBoundingBox();
        ~HotSpotPolygonDebuggerBoundingBox() override;

    public:
        bool _getBoundingBox( const HotSpotPolygonPtr & _node, mt::box2f * _bb ) const override;
    };
}
