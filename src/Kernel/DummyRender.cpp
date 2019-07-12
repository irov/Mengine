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
    void DummyRender::render( const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyRender::renderWithChildren( const RenderContext * _context, bool _external ) const
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

        context.viewport = m_renderViewport != nullptr ? m_renderViewport : _context->viewport;
        context.camera = m_renderCamera != nullptr ? m_renderCamera : _context->camera;
        context.transformation = m_renderTransformation != nullptr ? m_renderTransformation : _context->transformation;
        context.scissor = m_renderScissor != nullptr ? m_renderScissor : _context->scissor;
        context.target = m_renderTarget != nullptr ? m_renderTarget : _context->target;

        for( const BaseRender * child : m_renderChildren )
        {
            child->renderWithChildren( &context, false );
        }

        if( context.target != nullptr )
        {
            const RenderInterfacePtr & targetRender = this->makeTargetRender( &context );

            if( targetRender != nullptr )
            {
                targetRender->render( _context );
            }
        }
    }    
}
