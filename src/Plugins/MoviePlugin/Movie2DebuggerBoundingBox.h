#pragma once

#include "Movie2.h"

#include "Kernel/BaseDebuggerBoundingBox.h"

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
        bool _getBoundingBox( const Movie2Ptr & _node, mt::box2f * const _bb ) const override;
    };
}
