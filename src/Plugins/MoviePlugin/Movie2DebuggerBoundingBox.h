#pragma once

#include "Plugins/NodeDebuggerPlugin/NodeDebuggerBoundingBoxInterface.h"

#include "Movie2.h"

namespace Mengine
{
    class Movie2DebuggerBoundingBox
        : public BaseDebuggerBoundingBox<Movie2>
        , public Factorable
    {
    public:
        bool _updateBoundingBox( const Movie2Ptr & _node, mt::box2f * _bb ) override;
    };
}
