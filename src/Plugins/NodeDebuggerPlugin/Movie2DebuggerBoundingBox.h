#pragma once

#include "BaseDebuggerBoundingBox.h"

#include "Engine/Movie2.h"

namespace Mengine
{
    class Movie2DebuggerBoundingBox
        : public BaseDebuggerBoundingBox<Movie2>
        , public Factorable
    {
    public:
        bool _updateBoundingBox( const HotSpotPolygonPtr & _node, mt::box2f * _bb ) override;
    };
}
