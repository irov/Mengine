#pragma once

#include "Plugins/NodeDebuggerPlugin/NodeDebuggerBoundingBoxInterface.h"

#include "Movie2.h"

namespace Mengine
{
    class Movie2DebuggerBoundingBox
        : public BaseDebuggerBoundingBox<Movie2Ptr>
        , public Factorable
    {
    public:
        Movie2DebuggerBoundingBox();
        ~Movie2DebuggerBoundingBox() override;

    public:
        bool _getBoundingBox( const Movie2Ptr & _node, mt::box2f * _bb ) override;
    };
}
