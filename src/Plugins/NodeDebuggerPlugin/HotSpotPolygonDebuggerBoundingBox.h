#pragma once

#include "NodeDebuggerBoundingBoxInterface.h"

#include "Engine/HotSpotPolygon.h"

namespace Mengine
{
    class HotSpotPolygonDebuggerBoundingBox
        : public BaseDebuggerBoundingBox<HotSpotPolygon>
        , public Factorable
    {
    public:
        bool _updateBoundingBox( const HotSpotPolygonPtr & _node, mt::box2f * _bb ) override;
    };
}
