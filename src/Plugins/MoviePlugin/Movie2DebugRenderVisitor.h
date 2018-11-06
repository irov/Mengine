#pragma once

#include "Kernel/DebugRenderVisitor.h"

#include "Movie2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2DebugRenderVisitor
        : public DebugRenderVisitor
        , public ConcreteVisitor<Movie2>
    {
    public:
        Movie2DebugRenderVisitor();
        ~Movie2DebugRenderVisitor() override;

    protected:
        void accept( Movie2 * _node ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2DebugRenderVisitor> Movie2DebugRenderVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}