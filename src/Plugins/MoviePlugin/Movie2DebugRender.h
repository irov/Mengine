#pragma once

#include "Plugins/NodeDebugRenderPlugin/BaseDebugRender.h"

#include "Movie2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2DebugRender
        : public BaseDebugRenderT<Movie2>
    {
    public:
        Movie2DebugRender();
        ~Movie2DebugRender() override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, Movie2 * _node ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2DebugRender> Movie2DebugRenderVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}