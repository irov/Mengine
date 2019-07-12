#pragma once

#include "NodeDebuggerBoundingBoxInterface.h"

#include "Engine/HotSpotPolygon.h"

namespace Mengine
{
    class HotSpotPolygonDebuggerBoundingBox
        : public BaseDebuggerBoundingBox<HotSpotPolygonPtr>
        , public Factorable
    {
    public:
        bool _getBoundingBox( const HotSpotPolygonPtr & _node, mt::box2f * _bb ) override;
    };
}
