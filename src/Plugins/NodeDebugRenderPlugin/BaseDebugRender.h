#pragma once

#include "NodeDebugRenderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    template<class T>
    class BaseDebugRenderT
        : public NodeDebugRenderInterface
        , public Factorable
    {
    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, Node * _node ) override
        {
            T * t = Helper::staticNodeCast<T *>( _node );

            this->_render( _renderPipeline, _context, t );
        }

    protected:
        virtual void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, T * _t ) = 0;
    };
}