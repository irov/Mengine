#include "Kernel/DummyRender.h"

#include "Interface/RenderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DummyRender::DummyRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DummyRender::~DummyRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyRender::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyRender::renderWithChildren( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external ) const
    {
        if( this->isRendering() == false )
        {
            return;
        }

        if( m_externalRender == true && _external == false )
        {
            return;
        }

        RenderContext context;

        context.viewport = m_renderViewport != nullptr ? m_renderViewport.get() : _context->viewport;
        context.camera = m_renderCamera != nullptr ? m_renderCamera.get() : _context->camera;
        context.transformation = m_renderTransformation != nullptr ? m_renderTransformation.get() : _context->transformation;
        context.scissor = m_renderScissor != nullptr ? m_renderScissor.get() : _context->scissor;
        context.target = m_renderTarget != nullptr ? m_renderTarget.get() : _context->target;

        for( const BaseRender * child : m_renderChildren )
        {
            child->renderWithChildren( _renderPipeline, &context, false );
        }

        if( context.target != nullptr )
        {
            const RenderInterfacePtr & targetRender = this->makeTargetRender( &context );

            if( targetRender != nullptr )
            {
                targetRender->render( _renderPipeline, _context );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
